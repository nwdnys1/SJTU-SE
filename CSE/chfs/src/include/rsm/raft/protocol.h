#pragma once

#include "rsm/raft/log.h"
#include "rpc/msgpack.hpp"

namespace chfs
{

    const std::string RAFT_RPC_START_NODE = "start node";
    const std::string RAFT_RPC_STOP_NODE = "stop node";
    const std::string RAFT_RPC_NEW_COMMEND = "new commend";
    const std::string RAFT_RPC_CHECK_LEADER = "check leader";
    const std::string RAFT_RPC_IS_STOPPED = "check stopped";
    const std::string RAFT_RPC_SAVE_SNAPSHOT = "save snapshot";
    const std::string RAFT_RPC_GET_SNAPSHOT = "get snapshot";

    const std::string RAFT_RPC_REQUEST_VOTE = "request vote";
    const std::string RAFT_RPC_APPEND_ENTRY = "append entries";
    const std::string RAFT_RPC_INSTALL_SNAPSHOT = "install snapshot";

    struct RequestVoteArgs
    {
        /* Lab3: Your code here */
        int last_idx;
        int last_term;
        int cid;
        int cterm;

        MSGPACK_DEFINE(cid, cterm, last_idx, last_term)
    };

    struct RequestVoteReply
    {
        /* Lab3: Your code here */
        int vid;
        int vterm;
        bool vpromise;
        MSGPACK_DEFINE(vid, vterm, vpromise)
    };

    template <typename Command>
    struct AppendEntriesArgs
    {
        /* Lab3: Your code here */
        int prev_idx;
        int prev_term;
        int lterm;
        int lid;
        int lcommit;
        std::vector<Command> entries;
    };

    struct RpcAppendEntriesArgs
    {
        /* Lab3: Your code here */

        MSGPACK_DEFINE(

        )
    };

    template <typename Command>
    RpcAppendEntriesArgs transform_append_entries_args(const AppendEntriesArgs<Command> &arg)
    {
        /* Lab3: Your code here */
        return RpcAppendEntriesArgs();
    }

    template <typename Command>
    AppendEntriesArgs<Command> transform_rpc_append_entries_args(const RpcAppendEntriesArgs &rpc_arg)
    {
        /* Lab3: Your code here */
        return AppendEntriesArgs<Command>();
    }

    struct AppendEntriesReply
    {
        /* Lab3: Your code here */

        int term;
        int success;
        MSGPACK_DEFINE(term, success)
    };

    struct InstallSnapshotArgs
    {
        /* Lab3: Your code here */
        int lterm;
        int lid;
        int last_idx;
        int last_term;
        std::vector<u8> snapshot_data;
        MSGPACK_DEFINE(lterm, lid, last_idx, last_term, snapshot_data)
    };

    struct InstallSnapshotReply
    {
        /* Lab3: Your code here */

        MSGPACK_DEFINE(

        )
    };

} /* namespace chfs */