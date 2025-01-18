#include <string>
#include <utility>
#include <vector>
#include <mutex>
#include "librpc/client.h"
#include "librpc/server.h"
#include "distributed/client.h"

// Lab4: Free to modify this file
namespace mapReduce
{
    struct KeyVal
    {
        KeyVal(const std::string &key, const std::string &val) : key(key), val(val) {}
        KeyVal() {}
        std::string key;
        std::string val;
    };

    enum mr_tasktype
    {
        NONE = 0,
        MAP,
        REDUCE
    };

    std::vector<KeyVal> Map(const std::string &content);

    std::string Reduce(const std::string &key, const std::vector<std::string> &values);

    const std::string ASK_TASK = "ask_task";
    const std::string SUBMIT_TASK = "submit_task";

    struct MR_CoordinatorConfig
    {
        uint16_t port;
        std::string ip_address;
        std::string resultFile;
        std::shared_ptr<chfs::ChfsClient> client;

        MR_CoordinatorConfig(std::string ip_address, uint16_t port, std::shared_ptr<chfs::ChfsClient> client, std::string resultFile) : port(port), ip_address(std::move(ip_address)), resultFile(resultFile), client(std::move(client)) {}
    };

    class SequentialMapReduce
    {
    public:
        SequentialMapReduce(std::shared_ptr<chfs::ChfsClient> client, const std::vector<std::string> &files, std::string resultFile);
        void doWork();

    private:
        std::shared_ptr<chfs::ChfsClient> chfs_client;
        std::vector<std::string> files;
        std::string outPutFile;
    };

    class Coordinator
    {
    public:
        Coordinator(MR_CoordinatorConfig config, const std::vector<std::string> &files, int nReduce);
        std::tuple<int, int, std::vector<std::string>, std::string> askTask();
        int submitTask(int taskType, int index, std::string inter_file);
        bool Done();

    private:
        enum STATE
        {
            WAITING,
            EXECUTING,
            FINISHED
        };

        struct MapTask
        {
            std::vector<std::string> file;
            STATE state;
            MapTask(std::vector<std::string> files)
            {
                this->file = files;
                this->state = STATE::WAITING;
            }
        };

        struct ReduceTask
        {
            std::vector<std::string> files;
            STATE state;
            ReduceTask(std::vector<std::string> files)
            {
                this->files = files;
                this->state = STATE::WAITING;
            }
        };

        std::vector<std::string> files;                // input files
        std::mutex mtx;                                // mutex for task list
        bool isFinished;                               // if the job is finished
        int fin_map;                                   // number of finished map tasks
        int fin_reduce;                                // number of finished reduce tasks
        int worker_num;                                // number of reduce workers
        std::unique_ptr<chfs::RpcServer> rpc_server;   // rpc server
        std::shared_ptr<chfs::ChfsClient> chfs_client; // chfs client
        std::vector<MapTask> map_tasks;                // map tasks
        std::vector<ReduceTask> reduce_tasks;          // reduce tasks
        std::vector<std::string> inter_files;          // intermediate files
    };

    class Worker
    {
    public:
        explicit Worker(MR_CoordinatorConfig config);
        void doWork();
        void stop();

    private:
        void doMap(std::vector<std::string> &files, std::string inter_file);
        void doReduce(std::vector<std::string> &files);
        void doSubmit(mr_tasktype taskType, int index, std::string inter_file);

        std::string outPutFile;
        std::unique_ptr<chfs::RpcClient> mr_client;
        std::shared_ptr<chfs::ChfsClient> chfs_client;
        std::unique_ptr<std::thread> work_thread;
        bool shouldStop = false;
    };

    class Utils
    {
    public:
        // 从content中提取key-value对
        static std::vector<KeyVal> str2kv(std::string content)
        {
            std::vector<KeyVal> res;
            auto len = content.size();
            for (unsigned int i = 0; i < len; ++i)
            {
                if (!isalpha(content[i])) // 如果不是字母，跳过
                    continue;

                std::string word = "";      // 保存单词
                std::string num = "";       // 保存数字
                while (isalpha(content[i])) // 如果是字母，遇到了新的单词
                {
                    word.push_back(content[i]);
                    ++i;
                }

                while (content[i] == ' ') // 跳过空格
                    ++i;

                while (isdigit(content[i])) // 读取数字
                {
                    num.push_back(content[i]);
                    ++i;
                }

                res.emplace_back(word, num);
            }
            return res;
        }
        // 从文件中读取内容
        static void get_file_content(std::string filename, std::string &content, std::shared_ptr<chfs::ChfsClient> chfs_client)
        {
            auto inode_id = chfs_client->lookup(1, filename).unwrap();
            auto type_attr = chfs_client->get_type_attr(inode_id).unwrap();
            auto file_content = chfs_client->read_file(inode_id, 0, type_attr.second.size).unwrap();
            content.assign(file_content.begin(), file_content.end());
        }
        // 将key-value对写入文件
        static void write_file_content(std::string filename, std::vector<std::pair<std::string, std::string>> &file_content, std::shared_ptr<chfs::ChfsClient> chfs_client)
        {
            auto inode_id = chfs_client->lookup(1, filename).unwrap();
            std::string content = "";
            for (auto &item : file_content)
                content += item.first + " " + item.second + "\n";
            chfs_client->write_file(inode_id, 0, std::vector<chfs::u8>(content.begin(), content.end()));
        }
    };
}