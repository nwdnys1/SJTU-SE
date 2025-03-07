#pragma once

#include <atomic>
#include <mutex>
#include <chrono>
#include <thread>
#include <ctime>
#include <algorithm>
#include <thread>
#include <memory>
#include <stdarg.h>
#include <unistd.h>
#include <filesystem>

#include "rsm/state_machine.h"
#include "rsm/raft/log.h"
#include "rsm/raft/protocol.h"
#include "utils/thread_pool.h"
#include "librpc/server.h"
#include "librpc/client.h"
#include "block/manager.h"

namespace chfs
{

    enum class RaftRole
    {
        Follower,
        Candidate,
        Leader
    };

    struct RaftNodeConfig
    {
        int node_id;
        uint16_t port;
        std::string ip_address;
    };

    template <typename StateMachine, typename Command>
    class RaftNode
    {

#define RAFT_LOG(fmt, args...)                                                                                                       \
    do                                                                                                                               \
    {                                                                                                                                \
        auto now =                                                                                                                   \
            std::chrono::duration_cast<std::chrono::milliseconds>(                                                                   \
                std::chrono::system_clock::now().time_since_epoch())                                                                 \
                .count();                                                                                                            \
        char buf[512];                                                                                                               \
        sprintf(buf, "[%ld][%s:%d][node %d term %d role %d] " fmt "\n", now, __FILE__, __LINE__, my_id, current_term, role, ##args); \
        thread_pool->enqueue([=]() { std::cerr << buf; });                                                                           \
    } while (0);

    public:
        RaftNode(int node_id, std::vector<RaftNodeConfig> node_configs);
        ~RaftNode();

        /* interfaces for test */
        void set_network(std::map<int, bool> &network_availablility);
        void set_reliable(bool flag);
        int get_list_state_log_num();
        int rpc_count();
        std::vector<u8> get_snapshot_direct();

    private:
        /*
         * Start the raft node.
         * Please make sure all of the rpc request handlers have been registered before this method.
         */
        auto start() -> int;

        /*
         * Stop the raft node.
         */
        auto stop() -> int;

        /* Returns whether this node is the leader, you should also return the current term. */
        auto is_leader() -> std::tuple<bool, int>;

        /* Checks whether the node is stopped */
        auto is_stopped() -> bool;

        /*
         * Send a new command to the raft nodes.
         * The returned tuple of the method contains three values:
         * 1. bool:  True if this raft node is the leader that successfully appends the log,
         *      false If this node is not the leader.
         * 2. int: Current term.
         * 3. int: Log index.
         */
        auto new_command(std::vector<u8> cmd_data, int cmd_size) -> std::tuple<bool, int, int>;

        /* Save a snapshot of the state machine and compact the log. */
        auto save_snapshot() -> bool;

        /* Get a snapshot of the state machine */
        auto get_snapshot() -> std::vector<u8>;

        /* Internal RPC handlers */
        auto request_vote(RequestVoteArgs arg) -> RequestVoteReply;
        auto append_entries(RpcAppendEntriesArgs arg) -> AppendEntriesReply;
        auto install_snapshot(InstallSnapshotArgs arg) -> InstallSnapshotReply;

        /* RPC helpers */
        void send_request_vote(int target, RequestVoteArgs arg);
        void handle_request_vote_reply(int target, const RequestVoteArgs arg, const RequestVoteReply reply);

        void send_append_entries(int target, AppendEntriesArgs<Command> arg);
        void handle_append_entries_reply(int target, const AppendEntriesArgs<Command> arg, const AppendEntriesReply reply);

        void send_install_snapshot(int target, InstallSnapshotArgs arg);
        void handle_install_snapshot_reply(int target, const InstallSnapshotArgs arg, const InstallSnapshotReply reply);

        /* background workers */
        void run_background_ping();
        void run_background_election();
        void run_background_commit();
        void run_background_apply();

        /* Data structures */
        bool network_stat; /* for test */

        std::mutex mtx;         /* A big lock to protect the whole data structure. */
        std::mutex clients_mtx; /* A lock to protect RpcClient pointers */
        std::unique_ptr<ThreadPool> thread_pool;
        std::unique_ptr<RaftLog<Command>> log_storage; /* To persist the raft log. */
        std::unique_ptr<StateMachine> state;           /*  The state machine that applies the raft log, e.g. a kv store. */

        std::unique_ptr<RpcServer> rpc_server;                     /* RPC server to recieve and handle the RPC requests. */
        std::map<int, std::unique_ptr<RpcClient>> rpc_clients_map; /* RPC clients of all raft nodes including this node. */
        std::vector<RaftNodeConfig> node_configs;                  /* Configuration for all nodes */
        int my_id;                                                 /* The index of this node in rpc_clients, start from 0. */

        std::atomic_bool stopped;

        RaftRole role;
        int current_term;
        int lid;

        std::unique_ptr<std::thread> background_election;
        std::unique_ptr<std::thread> background_ping;
        std::unique_ptr<std::thread> background_commit;
        std::unique_ptr<std::thread> background_apply;

        /* Lab3: Your code here */
        unsigned long election_time;
        int agree_num;
        unsigned long timeout;
        unsigned long last_hbt;
        unsigned long election_timeout;
        std::shared_ptr<BlockManager> bm;
        auto get_time() -> unsigned long;
        auto restart_timeout(int max, int min) -> unsigned long;
    };

    template <typename StateMachine, typename Command>
    RaftNode<StateMachine, Command>::RaftNode(int node_id, std::vector<RaftNodeConfig> configs) : network_stat(true),
                                                                                                  node_configs(configs),
                                                                                                  my_id(node_id),
                                                                                                  stopped(true),
                                                                                                  role(RaftRole::Follower),
                                                                                                  current_term(0),
                                                                                                  lid(-1)
    {
        auto my_config = node_configs[my_id];

        /* launch RPC server */
        rpc_server = std::make_unique<RpcServer>(my_config.ip_address, my_config.port);

        /* Register the RPCs. */
        rpc_server->bind(RAFT_RPC_START_NODE, [this]()
                         { return this->start(); });
        rpc_server->bind(RAFT_RPC_STOP_NODE, [this]()
                         { return this->stop(); });
        rpc_server->bind(RAFT_RPC_CHECK_LEADER, [this]()
                         { return this->is_leader(); });
        rpc_server->bind(RAFT_RPC_IS_STOPPED, [this]()
                         { return this->is_stopped(); });
        rpc_server->bind(RAFT_RPC_NEW_COMMEND, [this](std::vector<u8> data, int cmd_size)
                         { return this->new_command(data, cmd_size); });
        rpc_server->bind(RAFT_RPC_SAVE_SNAPSHOT, [this]()
                         { return this->save_snapshot(); });
        rpc_server->bind(RAFT_RPC_GET_SNAPSHOT, [this]()
                         { return this->get_snapshot(); });

        rpc_server->bind(RAFT_RPC_REQUEST_VOTE, [this](RequestVoteArgs arg)
                         { return this->request_vote(arg); });
        rpc_server->bind(RAFT_RPC_APPEND_ENTRY, [this](RpcAppendEntriesArgs arg)
                         { return this->append_entries(arg); });
        rpc_server->bind(RAFT_RPC_INSTALL_SNAPSHOT, [this](InstallSnapshotArgs arg)
                         { return this->install_snapshot(arg); });

        /* Lab3: Your code here */
        // initialize the data
        this->thread_pool = std::make_unique<ThreadPool>(32);
        this->state = std::make_unique<StateMachine>();
        this->agree_num = 0;
        this->last_hbt = this->get_time();
        this->election_time = this->get_time();
        this->election_timeout = this->restart_timeout(300, 150);
        this->timeout = this->restart_timeout(300, 150);
        rpc_server->run(true, configs.size());
    }

    template <typename StateMachine, typename Command>
    RaftNode<StateMachine, Command>::~RaftNode()
    {
        stop();
        thread_pool.reset();
        rpc_server.reset();
        state.reset();
        log_storage.reset();
        this->bm.reset();
    }

    /******************************************************************

                            RPC Interfaces

    *******************************************************************/

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::get_time() -> unsigned long
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::restart_timeout(int max, int min) -> unsigned long
    {
        // 随机化timeout
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<unsigned long> distribution(min, max);
        return distribution(generator);
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::start() -> int
    {
        /* Lab3: Your code here */
        this->rpc_clients_map.clear();
        for (auto &item : this->node_configs)
            this->rpc_clients_map.insert(std::make_pair(item.node_id, std::make_unique<RpcClient>(item.ip_address, item.port, true)));

        this->stopped.store(false);
        background_election = std::make_unique<std::thread>(&RaftNode::run_background_election, this);
        background_ping = std::make_unique<std::thread>(&RaftNode::run_background_ping, this);
        background_commit = std::make_unique<std::thread>(&RaftNode::run_background_commit, this);
        background_apply = std::make_unique<std::thread>(&RaftNode::run_background_apply, this);

        return 0;
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::stop() -> int
    {
        /* Lab3: Your code here */
        this->stopped.store(true);
        this->background_election->join();
        this->background_ping->join();
        this->background_commit->join();
        this->background_apply->join();
        return 0;
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::is_leader() -> std::tuple<bool, int>
    {
        /* Lab3: Your code here */
        std::lock_guard<std::mutex> lock(this->mtx);
        return std::make_tuple(this->role == RaftRole::Leader, this->current_term);
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::is_stopped() -> bool
    {
        return stopped.load();
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::new_command(std::vector<u8> cmd_data, int cmd_size) -> std::tuple<bool, int, int>
    {
        /* Lab3: Your code here */
        return std::make_tuple(false, -1, -1);
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::save_snapshot() -> bool
    {
        /* Lab3: Your code here */
        return true;
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::get_snapshot() -> std::vector<u8>
    {
        /* Lab3: Your code here */
        return std::vector<u8>();
    }

    /******************************************************************

                             Internal RPC Related

    *******************************************************************/

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::request_vote(RequestVoteArgs args) -> RequestVoteReply
    {
        /* Lab3: Your code here */
        RequestVoteReply reply;
        std::unique_lock<std::mutex> lock(this->mtx);
        if (args.cterm > this->current_term)
        {
            this->current_term = args.cterm;
            this->lid = -1;
            this->role = RaftRole::Follower;
            this->agree_num = 0;
            this->timeout = this->restart_timeout(300, 150);
            this->last_hbt = this->get_time();
        }
        reply.vid = this->my_id;
        reply.vterm = this->current_term;
        bool term_bo = (this->current_term == args.cterm);
        bool votefor_bo = (this->lid == args.cid) || (this->lid == -1);
        if (term_bo && votefor_bo)
        {
            this->last_hbt = this->get_time();
            reply.vpromise = true;
            this->lid = args.cid;
            this->role = RaftRole::Follower;
        }
        else
            reply.vpromise = false;

        lock.unlock();
        return reply;
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::handle_request_vote_reply(int target, const RequestVoteArgs arg, const RequestVoteReply reply)
    {
        /* Lab3: Your code here */
        std::unique_lock<std::mutex> lock(this->mtx);
        if (reply.vterm > this->current_term)
        {
            this->current_term = reply.vterm;
            this->lid = -1;
            this->role = RaftRole::Follower;
            this->agree_num = 0;
            this->timeout = this->restart_timeout(300, 150);
            this->last_hbt = this->get_time();
        }
        if (reply.vpromise)
        {
            this->agree_num += 1;
            if (this->role == RaftRole::Candidate && this->agree_num >= (this->rpc_clients_map.size() / 2 + 1))
                this->role = RaftRole::Leader;
        }

        lock.unlock();
        return;
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::append_entries(RpcAppendEntriesArgs rpc_arg) -> AppendEntriesReply
    {
        /* Lab3: Your code here */
        AppendEntriesReply result;
        result.success = false;
        AppendEntriesArgs<Command> arg = transform_rpc_append_entries_args<Command>(rpc_arg);
        std::unique_lock<std::mutex> lock(this->mtx);
        if (arg.lterm >= this->current_term)
        {
            this->current_term = arg.lterm;
            this->role = RaftRole::Follower;
            this->agree_num = 0;
            this->lid = arg.lid;
            this->timeout = this->restart_timeout(300, 150);
            this->last_hbt = this->get_time();
        }
        if (this->current_term == arg.lterm)
        {
            this->last_hbt = this->get_time();
            if (arg.entries.empty())
            {
                lock.unlock();
                result.term = current_term;
                return result;
            }
        }

        lock.unlock();
        result.term = current_term;
        return result;
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::handle_append_entries_reply(int node_id, const AppendEntriesArgs<Command> arg, const AppendEntriesReply reply)
    {
        /* Lab3: Your code here */
        std::unique_lock<std::mutex> lock(this->mtx);
        if (reply.term > this->current_term)
        {
            this->current_term = reply.term;
            this->role = RaftRole::Follower;
            this->lid = -1;
            this->timeout = this->restart_timeout(300, 150);
            this->last_hbt = this->get_time();
        }
        lock.unlock();
        return;
    }

    template <typename StateMachine, typename Command>
    auto RaftNode<StateMachine, Command>::install_snapshot(InstallSnapshotArgs args) -> InstallSnapshotReply
    {
        /* Lab3: Your code here */
        return InstallSnapshotReply();
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::handle_install_snapshot_reply(int node_id, const InstallSnapshotArgs arg, const InstallSnapshotReply reply)
    {
        /* Lab3: Your code here */
        return;
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::send_request_vote(int target_id, RequestVoteArgs arg)
    {
        std::unique_lock<std::mutex> clients_lock(clients_mtx);
        if (rpc_clients_map[target_id] == nullptr || rpc_clients_map[target_id]->get_connection_state() != rpc::client::connection_state::connected)
            return;
        auto res = rpc_clients_map[target_id]->call(RAFT_RPC_REQUEST_VOTE, arg);
        clients_lock.unlock();
        if (res.is_ok())
            handle_request_vote_reply(target_id, arg, res.unwrap()->as<RequestVoteReply>());
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::send_append_entries(int target_id, AppendEntriesArgs<Command> arg)
    {
        std::unique_lock<std::mutex> clients_lock(clients_mtx);
        if (rpc_clients_map[target_id] == nullptr || rpc_clients_map[target_id]->get_connection_state() != rpc::client::connection_state::connected)
        {
            return;
        }

        RpcAppendEntriesArgs rpc_arg = transform_append_entries_args(arg);
        auto res = rpc_clients_map[target_id]->call(RAFT_RPC_APPEND_ENTRY, rpc_arg);
        clients_lock.unlock();
        if (res.is_ok())
        {
            handle_append_entries_reply(target_id, arg, res.unwrap()->as<AppendEntriesReply>());
        }
        else
        {
            // RPC fails
        }
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::send_install_snapshot(int target_id, InstallSnapshotArgs arg)
    {
        std::unique_lock<std::mutex> clients_lock(clients_mtx);
        if (rpc_clients_map[target_id] == nullptr || rpc_clients_map[target_id]->get_connection_state() != rpc::client::connection_state::connected)
        {
            return;
        }

        auto res = rpc_clients_map[target_id]->call(RAFT_RPC_INSTALL_SNAPSHOT, arg);
        clients_lock.unlock();
        if (res.is_ok())
        {
            handle_install_snapshot_reply(target_id, arg, res.unwrap()->as<InstallSnapshotReply>());
        }
        else
        {
            // RPC fails
        }
    }

    /******************************************************************

                            Background Workers

    *******************************************************************/

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::run_background_election()
    {
        // Periodly check the liveness of the leader.

        // Work for followers and candidates.
        while (true)
        {
            {
                if (this->is_stopped())
                {
                    return;
                }
                /* Lab3: Your code here */
                std::unique_lock<std::mutex> lock(this->mtx);
                bool follow_over = ((this->role == RaftRole::Follower) && ((this->get_time() - this->last_hbt) > this->timeout));
                bool candidate_over = ((this->role == RaftRole::Candidate) && ((this->get_time() - this->election_time) > this->election_timeout));
                if (follow_over || candidate_over)
                {
                    this->role = RaftRole::Candidate;
                    this->current_term += 1;
                    this->agree_num = 1;
                    this->lid = this->my_id;
                    this->election_time = this->get_time();
                    this->election_timeout = this->restart_timeout(300, 150);
                    for (int i = 0; i < this->rpc_clients_map.size(); ++i)
                    {
                        if (i == this->my_id)
                            continue;
                        RequestVoteArgs request_vote_arg;
                        request_vote_arg.cid = this->my_id;
                        request_vote_arg.cterm = this->current_term;
                        this->thread_pool->enqueue(&RaftNode::send_request_vote, this, i, request_vote_arg);
                    }
                }
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        return;
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::run_background_commit()
    {
        // Periodly send logs to the follower.

        // Only work for the leader.

        /* Uncomment following code when you finish */
        // while (true) {
        //     {
        //         if (is_stopped()) {
        //             return;
        //         }
        //         /* Lab3: Your code here */
        //     }
        // }

        return;
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::run_background_apply()
    {
        // Periodly apply committed logs the state machine

        // Work for all the nodes.

        /* Uncomment following code when you finish */
        // while (true) {
        //     {
        //         if (is_stopped()) {
        //             return;
        //         }
        //         /* Lab3: Your code here */
        //     }
        // }

        return;
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::run_background_ping()
    {
        // Periodly send empty append_entries RPC to the followers.

        // Only work for the leader.

        while (true)
        {
            {
                if (is_stopped())
                    return;

                /* Lab3: Your code here */
                std::unique_lock<std::mutex> lock(this->mtx);
                if (this->role == RaftRole::Leader)
                {
                    for (int i = 0; i < this->rpc_clients_map.size(); ++i)
                    {
                        if (i == this->my_id)
                            continue;
                        AppendEntriesArgs<Command> tmp;
                        tmp.lterm = this->current_term;
                        tmp.lid = this->my_id;
                        this->thread_pool->enqueue(&RaftNode::send_append_entries, this, i, tmp);
                    }
                }
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
        return;
    }

    /******************************************************************

                              Test Functions (must not edit)

    *******************************************************************/

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::set_network(std::map<int, bool> &network_availability)
    {
        std::unique_lock<std::mutex> clients_lock(clients_mtx);

        /* turn off network */
        if (!network_availability[my_id])
        {
            for (auto &&client : rpc_clients_map)
            {
                if (client.second != nullptr)
                    client.second.reset();
            }

            return;
        }

        for (auto node_network : network_availability)
        {
            int node_id = node_network.first;
            bool node_status = node_network.second;

            if (node_status && rpc_clients_map[node_id] == nullptr)
            {
                RaftNodeConfig target_config;
                for (auto config : node_configs)
                {
                    if (config.node_id == node_id)
                        target_config = config;
                }

                rpc_clients_map[node_id] = std::make_unique<RpcClient>(target_config.ip_address, target_config.port, true);
            }

            if (!node_status && rpc_clients_map[node_id] != nullptr)
            {
                rpc_clients_map[node_id].reset();
            }
        }
    }

    template <typename StateMachine, typename Command>
    void RaftNode<StateMachine, Command>::set_reliable(bool flag)
    {
        std::unique_lock<std::mutex> clients_lock(clients_mtx);
        for (auto &&client : rpc_clients_map)
        {
            if (client.second)
            {
                client.second->set_reliable(flag);
            }
        }
    }

    template <typename StateMachine, typename Command>
    int RaftNode<StateMachine, Command>::get_list_state_log_num()
    {
        /* only applied to ListStateMachine*/
        std::unique_lock<std::mutex> lock(mtx);

        return state->num_append_logs;
    }

    template <typename StateMachine, typename Command>
    int RaftNode<StateMachine, Command>::rpc_count()
    {
        int sum = 0;
        std::unique_lock<std::mutex> clients_lock(clients_mtx);

        for (auto &&client : rpc_clients_map)
        {
            if (client.second)
            {
                sum += client.second->count();
            }
        }

        return sum;
    }

    template <typename StateMachine, typename Command>
    std::vector<u8> RaftNode<StateMachine, Command>::get_snapshot_direct()
    {
        if (is_stopped())
        {
            return std::vector<u8>();
        }

        std::unique_lock<std::mutex> lock(mtx);

        return state->snapshot();
    }

}