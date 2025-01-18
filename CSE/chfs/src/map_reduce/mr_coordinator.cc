#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mutex>

#include "map_reduce/protocol.h"

namespace mapReduce
{
  std::tuple<int, int, std::vector<std::string>, std::string> Coordinator::askTask()
  {
    // Lab4 : Your code goes here.
    // Free to change the type of return value.
    std::unique_lock<std::mutex> lock(this->mtx);

    if (this->isFinished) // 任务已完成
    {
      lock.unlock();
      return std::make_tuple(-2, -1, std::vector<std::string>(), ""); // 返回-2表示任务已完成
    }

    if (fin_map < map_tasks.size()) // 还有map任务未完成
    {
      for (int i = 0; i < map_tasks.size(); ++i)  // 分配map任务
        if (map_tasks[i].state == STATE::WAITING) // 找到一个未执行的map任务
        {
          map_tasks[i].state = STATE::EXECUTING;                    // 修改任务状态为执行中
          std::string inter_file = "inter_" + map_tasks[i].file[0]; // 创建中间文件
          chfs_client->mknode(chfs::ChfsClient::FileType::REGULAR, 1, "inter_" + map_tasks[i].file[0]);
          lock.unlock();
          return std::make_tuple(i, (int)(mr_tasktype::MAP), map_tasks[i].file, inter_file); // 返回任务信息
        }
    }
    else // 还有reduce任务未完成
    {
      for (int i = 0; i < reduce_tasks.size(); ++i)
      {
        if (reduce_tasks[i].state == STATE::WAITING)
        {
          reduce_tasks[i].state = STATE::EXECUTING;
          lock.unlock();
          return std::make_tuple(i, (int)(mr_tasktype::REDUCE), reduce_tasks[i].files, "");
        }
      }
    }
    // 没有任务
    lock.unlock();
    return std::make_tuple(-1, -1, std::vector<std::string>(), "");
  }

  int Coordinator::submitTask(int taskType, int index, std::string inter_file)
  {
    // Lab4 : Your code goes here.
    std::unique_lock<std::mutex> lock(this->mtx);

    if ((mr_tasktype)(taskType) == mr_tasktype::MAP) // map 任务完成
    {
      map_tasks[index].state = STATE::FINISHED; // 修改对应任务状态为完成
      inter_files.push_back(inter_file);        // 将中间文件名加入中间文件列表
      if (++fin_map == map_tasks.size())        // 如果所有map任务完成
      {
        reduce_tasks.emplace_back(inter_files); // 创建reduce任务
        fin_reduce = 0;
      }
    }
    else // reduce 任务完成
    {
      reduce_tasks[index].state = STATE::FINISHED; // 修改对应任务状态为完成
      if (++fin_reduce == reduce_tasks.size())
        this->isFinished = true; // 如果所有reduce任务完成，任务完成
    }

    lock.unlock();
    return 0;
  }

  // mr_coordinator calls Done() periodically to find out
  // if the entire job has finished.
  bool Coordinator::Done()
  {
    std::unique_lock<std::mutex> uniqueLock(this->mtx);
    return this->isFinished;
  }

  // create a Coordinator.
  // nReduce is the number of reduce tasks to use.
  Coordinator::Coordinator(MR_CoordinatorConfig config, const std::vector<std::string> &files, int nReduce)
  {
    this->files = files;
    this->isFinished = false;
    // Lab4: Your code goes here (Optional).

    this->fin_map = 0;
    this->fin_reduce = 0;
    this->worker_num = nReduce;
    map_tasks.emplace_back(files);
    chfs_client = config.client;

    rpc_server = std::make_unique<chfs::RpcServer>(config.ip_address, config.port);
    rpc_server->bind(ASK_TASK, [this]()
                     { return this->askTask(); });
    rpc_server->bind(SUBMIT_TASK, [this](int taskType, int index, std::string inter_file)
                     { return this->submitTask(taskType, index, inter_file); });
    rpc_server->run(true, 1);
  }
}