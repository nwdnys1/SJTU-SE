#include "distributed/metadata_server.h"
#include "common/util.h"
#include "filesystem/directory_op.h"
#include <fstream>

namespace chfs
{

  inline auto MetadataServer::bind_handlers()
  {
    server_->bind("mknode",
                  [this](u8 type, inode_id_t parent, std::string const &name)
                  {
                    return this->mknode(type, parent, name);
                  });
    server_->bind("unlink", [this](inode_id_t parent, std::string const &name)
                  { return this->unlink(parent, name); });
    server_->bind("lookup", [this](inode_id_t parent, std::string const &name)
                  { return this->lookup(parent, name); });
    server_->bind("get_block_map",
                  [this](inode_id_t id)
                  { return this->get_block_map(id); });
    server_->bind("alloc_block",
                  [this](inode_id_t id)
                  { return this->allocate_block(id); });
    server_->bind("free_block",
                  [this](inode_id_t id, block_id_t block, mac_id_t machine_id)
                  {
                    return this->free_block(id, block, machine_id);
                  });
    server_->bind("readdir", [this](inode_id_t id)
                  { return this->readdir(id); });
    server_->bind("get_type_attr",
                  [this](inode_id_t id)
                  { return this->get_type_attr(id); });
  }

  inline auto MetadataServer::init_fs(const std::string &data_path)
  {
    /**
     * Check whether the metadata exists or not.
     * If exists, we wouldn't create one from scratch.
     */
    bool is_initialed = is_file_exist(data_path);

    auto block_manager = std::shared_ptr<BlockManager>(nullptr);
    if (is_log_enabled_)
    {
      block_manager =
          std::make_shared<BlockManager>(data_path, KDefaultBlockCnt, true);
    }
    else
    {
      block_manager = std::make_shared<BlockManager>(data_path, KDefaultBlockCnt);
    }

    CHFS_ASSERT(block_manager != nullptr, "Cannot create block manager.");

    if (is_initialed)
    {
      auto origin_res = FileOperation::create_from_raw(block_manager);
      std::cout << "Restarting..." << std::endl;
      if (origin_res.is_err())
      {
        std::cerr << "Original FS is bad, please remove files manually."
                  << std::endl;
        exit(1);
      }

      operation_ = origin_res.unwrap();
    }
    else
    {
      operation_ = std::make_shared<FileOperation>(block_manager,
                                                   DistributedMaxInodeSupported);
      std::cout << "We should init one new FS..." << std::endl;
      /**
       * If the filesystem on metadata server is not initialized, create
       * a root directory.
       */
      auto init_res = operation_->alloc_inode(InodeType::Directory);
      if (init_res.is_err())
      {
        std::cerr << "Cannot allocate inode for root directory." << std::endl;
        exit(1);
      }

      CHFS_ASSERT(init_res.unwrap() == 1, "Bad initialization on root dir.");
    }

    running = false;
    num_data_servers =
        0; // Default no data server. Need to call `reg_server` to add.

    if (is_log_enabled_)
    {
      if (may_failed_)
        operation_->block_manager_->set_may_fail(true);
      commit_log = std::make_shared<CommitLog>(operation_->block_manager_,
                                               is_checkpoint_enabled_);
    }

    bind_handlers();

    /**
     * The metadata server wouldn't start immediately after construction.
     * It should be launched after all the data servers are registered.
     */
  }

  MetadataServer::MetadataServer(u16 port, const std::string &data_path,
                                 bool is_log_enabled, bool is_checkpoint_enabled,
                                 bool may_failed)
      : is_log_enabled_(is_log_enabled), may_failed_(may_failed),
        is_checkpoint_enabled_(is_checkpoint_enabled)
  {
    server_ = std::make_unique<RpcServer>(port);
    init_fs(data_path);
    if (is_log_enabled_)
    {
      commit_log = std::make_shared<CommitLog>(operation_->block_manager_,
                                               is_checkpoint_enabled);
    }
  }

  MetadataServer::MetadataServer(std::string const &address, u16 port,
                                 const std::string &data_path,
                                 bool is_log_enabled, bool is_checkpoint_enabled,
                                 bool may_failed)
      : is_log_enabled_(is_log_enabled), may_failed_(may_failed),
        is_checkpoint_enabled_(is_checkpoint_enabled)
  {
    server_ = std::make_unique<RpcServer>(address, port);
    init_fs(data_path);
    if (is_log_enabled_)
    {
      commit_log = std::make_shared<CommitLog>(operation_->block_manager_,
                                               is_checkpoint_enabled);
    }
  }

  // {Your code here}
  auto MetadataServer::mknode(u8 type, inode_id_t parent, const std::string &name)
      -> inode_id_t
  {
    // TODO: Implement this function.

    // use mutex to protect the metadata
    this->metadata_mutex.lock();
    if (this->is_log_enabled_)
    {
      this->commit_log->checkpoint();
      this->commit_log->log_entry_num++; // increase the log entry number
    }
    // create a new file or directory
    ChfsResult<inode_id_t> res = this->operation_->mk_helper(parent, name.data(), (chfs::InodeType)type);
    this->metadata_mutex.unlock();
    // return the result
    if (res.is_ok())
    {
      if (this->is_log_enabled_)
        this->commit_log->commit_log(this->commit_log->log_entry_num); // commit the log
      if (this->may_failed_)
        return 0;
      return res.unwrap();
    }
    else // if error
      return 0;
  }

  // if error, commit the log and unlock the metadata mutex, then return false
  bool MetadataServer::commit_and_unlock()
  {
    if (this->is_log_enabled_)
      this->commit_log->commit_log(this->commit_log->log_entry_num);
    this->metadata_mutex.unlock();
    return false;
  }

  // {Your code here}
  auto MetadataServer::unlink(inode_id_t parent, const std::string &name)
      -> bool
  {
    // TODO: Implement this function.
    // use mutex to protect the metadata
    this->metadata_mutex.lock();
    if (this->is_log_enabled_)
    {
      this->commit_log->checkpoint();
      this->commit_log->log_entry_num++;
    }
    // get the inode id of the file to be unlinked
    ChfsResult<inode_id_t> inode_id = this->operation_->lookup(parent, name.data());
    if (inode_id.is_err())
      return this->commit_and_unlock();
    // get the type of the file to be unlinked
    ChfsResult<InodeType> inode_type = this->operation_->gettype(inode_id.unwrap());
    if (inode_type.is_err())
      return this->commit_and_unlock();

    // if the file to be unlinked is a directory
    if (inode_type.unwrap() == InodeType::Directory)
    {
      if (this->operation_->unlink(parent, name.data()).is_err()) // delete the directory
        return this->commit_and_unlock();
    }
    else // if the file to be unlinked is a regular file
    {
      // get the block map of the file to be unlinked
      std::vector<BlockInfo> free_block_list;
      this->operation_->get_blockinfo_list(inode_id.unwrap(), free_block_list);
      for (auto item : free_block_list)
      {
        auto machine_id = std::get<1>(item); // get the machine id of the block
        auto block_id = std::get<0>(item);   // get the block id of the block
        // call the rpc function to free the block
        auto rpc_res = (this->clients_).at(machine_id)->call("free_block", block_id);
        // if error or the rpc function returns false
        if (rpc_res.is_err() || !rpc_res.unwrap()->as<bool>())
          return this->commit_and_unlock();
      }
      // delete the file from the parent directory
      ChfsNullResult unlink_res = this->operation_->unlink_block(parent, name);
      if (unlink_res.is_err())
        return this->commit_and_unlock();
    }
    commit_and_unlock(); // commit the log and unlock the metadata mutex
    return this->may_failed_ ? false : true;
  }

  // {Your code here}
  auto MetadataServer::lookup(inode_id_t parent, const std::string &name) -> inode_id_t
  {
    // TODO: Implement this function.
    // use the lookup in lab1
    ChfsResult<inode_id_t> res = this->operation_->lookup(parent, name.data());
    if (res.is_ok())
      return res.unwrap();
    else
      return 0;
  }

  // {Your code here}
  auto MetadataServer::get_block_map(inode_id_t id) -> std::vector<BlockInfo>
  {
    // TODO: Implement this function.
    std::vector<BlockInfo> res;
    this->operation_->get_blockinfo_list(id, res);
    return res;
  }

  // {Your code here}
  auto MetadataServer::allocate_block(inode_id_t id) -> BlockInfo
  {
    // TODO: Implement this function.
    // use mutex to protect the metadata
    metadata_mutex.lock();
    // get a random machine id
    mac_id_t machine_id = (this->generator).rand(1, this->num_data_servers);
    // call the rpc function to allocate a block
    auto rpc_res = (this->clients_).at(machine_id)->call("alloc_block");
    if (rpc_res.is_err())
    {
      metadata_mutex.unlock(); // remember to unlock the mutex
      return BlockInfo(0, 0, 0);
    }
    // get the block id and version
    auto [block_id, version] = rpc_res.unwrap()->as<std::pair<block_id_t, version_t>>();
    // add the block info to the metadata
    BlockInfo res(block_id, machine_id, version);
    this->operation_->add_blockinfo(id, res);
    metadata_mutex.unlock();
    return res;
  }

  // {Your code here}
  auto MetadataServer::free_block(inode_id_t id, block_id_t block_id, mac_id_t machine_id) -> bool
  {
    // TODO: Implement this function.
    // use mutex to protect the metadata
    metadata_mutex.lock();
    auto rpc_res = (this->clients_).at(machine_id)->call("free_block", block_id);
    // if the rpc function returns false or error
    if (rpc_res.is_err() || !rpc_res.unwrap()->as<bool>())
    {
      metadata_mutex.unlock();
      return false;
    }
    // remove the block info from the metadata
    ChfsNullResult res = this->operation_->remove_blockinfo(id, block_id, machine_id);
    metadata_mutex.unlock();
    return res.is_ok();
  }

  // {Your code here}
  auto MetadataServer::readdir(inode_id_t node) -> std::vector<std::pair<std::string, inode_id_t>>
  {
    // TODO: Implement this function.
    // use the read_directory in lab1
    std::list<chfs::DirectoryEntry> file_list;
    if (read_directory((this->operation_).get(), node, file_list).is_ok())
    {
      std::vector<std::pair<std::string, inode_id_t>> res;
      for (auto it = file_list.begin(); it != file_list.end(); ++it)
      {
        auto file = std::pair<std::string, inode_id_t>(it->name, it->id); // get the file name and inode id
        res.push_back(file);
      }
      return res;
    }
    else // if error
      return std::vector<std::pair<std::string, inode_id_t>>();
  }

  // {Your code here}
  auto MetadataServer::get_type_attr(inode_id_t id) -> std::tuple<u64, u64, u64, u64, u8>
  {
    // TODO: Implement this function.
    // get the type and attribute of the file
    ChfsResult<std::pair<InodeType, FileAttr>> res = this->operation_->get_type_attr(id);
    if (res.is_ok())
    {
      std::pair<InodeType, FileAttr> type_attr = res.unwrap();
      return std::tuple<u64, u64, u64, u64, u8>(type_attr.second.size, type_attr.second.atime, type_attr.second.mtime, type_attr.second.ctime, (u8)(type_attr.first));
    }
    else // if error
      return std::tuple<u64, u64, u64, u64, u8>(0, 0, 0, 0, 0);
  }

  auto MetadataServer::reg_server(const std::string &address, u16 port, bool reliable) -> bool
  {
    num_data_servers += 1;
    auto cli = std::make_shared<RpcClient>(address, port, reliable);
    clients_.insert(std::make_pair(num_data_servers, cli));

    return true;
  }

  auto MetadataServer::run() -> bool
  {
    if (running)
      return false;

    // Currently we only support async start
    server_->run(true, num_worker_threads);
    running = true;
    return true;
  }

} // namespace chfs