#include "distributed/dataserver.h"
#include "common/util.h"

namespace chfs
{

  auto DataServer::initialize(std::string const &data_path)
  {
    /**
     * At first check whether the file exists or not.
     * If so, which means the distributed chfs has
     * already been initialized and can be rebuilt from
     * existing data.
     */
    bool is_initialized = is_file_exist(data_path);

    auto bm = std::shared_ptr<BlockManager>(
        new BlockManager(data_path, KDefaultBlockCnt));
    if (is_initialized)
    {
      block_allocator_ =
          std::make_shared<BlockAllocator>(bm, 0, false);
    }
    else
    {
      // We need to reserve some blocks for storing the version of each block
      block_allocator_ = std::shared_ptr<BlockAllocator>(
          new BlockAllocator(bm, 0, true));
    }

    // Initialize the RPC server and bind all handlers
    server_->bind("read_data", [this](block_id_t block_id, usize offset,
                                      usize len, version_t version)
                  { return this->read_data(block_id, offset, len, version); });
    server_->bind("write_data", [this](block_id_t block_id, usize offset,
                                       std::vector<u8> &buffer)
                  { return this->write_data(block_id, offset, buffer); });
    server_->bind("alloc_block", [this]()
                  { return this->alloc_block(); });
    server_->bind("free_block", [this](block_id_t block_id)
                  { return this->free_block(block_id); });

    // Launch the rpc server to listen for requests
    server_->run(true, num_worker_threads);
  }

  DataServer::DataServer(u16 port, const std::string &data_path)
      : server_(std::make_unique<RpcServer>(port))
  {
    initialize(data_path);
  }

  DataServer::DataServer(std::string const &address, u16 port,
                         const std::string &data_path)
      : server_(std::make_unique<RpcServer>(address, port))
  {
    initialize(data_path);
  }

  DataServer::~DataServer() { server_.reset(); }

  // {Your code here}
  auto DataServer::read_data(block_id_t block_id, usize offset, usize len,
                             version_t version) -> std::vector<u8>
  {
    // TODO: Implement this function.

    usize block_size = this->block_allocator_->bm->block_size();
    // check the offset and len, if it's out of range:
    if (offset + len > block_size)
      return std::vector<u8>(0); // return 0-size vector

    // check the version
    std::vector<u8> buffer(block_size);
    u64 version_id = block_id / (block_size / sizeof(version_t));
    u64 version_offset = (block_id % (block_size / sizeof(version_t))) * sizeof(version_t);
    // read the version
    ChfsNullResult version_res = this->block_allocator_->bm->read_block(version_id, buffer.data());
    // if the version is invalid:
    if (version_res.is_err())
      return std::vector<u8>(0); // return 0-size vector

    version_t current_version = *(reinterpret_cast<version_t *>(buffer.data() + version_offset));
    // if the version is invalid:
    if (current_version > version)
      return std::vector<u8>(0); // return 0-size vector

    // read the data
    ChfsNullResult result = this->block_allocator_->bm->read_block(block_id, buffer.data());
    if (result.is_ok())
    {
      std::vector<u8> data(len);
      auto start = buffer.begin() + offset;
      std::copy(start, start + len, data.begin());
      return data;
    }
    // if the read operation failed
    else
      return std::vector<u8>(0); // return 0-size vector
  }

  // {Your code here}
  auto DataServer::write_data(block_id_t block_id, usize offset,
                              std::vector<u8> &buffer) -> bool
  {
    // TODO: Implement this function.

    usize block_size = this->block_allocator_->bm->block_size();
    usize len = buffer.size();

    // check the offset and len, if it's out of range:
    if (offset + len > block_size)
      return false; // return false
    // write the data
    ChfsNullResult result = this->block_allocator_->bm->write_partial_block(block_id, buffer.data(), offset, len);
    if (result.is_ok())
      return true;
    else
      return false;
  }

  // {Your code here}
  auto DataServer::alloc_block() -> std::pair<block_id_t, version_t>
  {
    // TODO: Implement this function.
    // allocate a block
    ChfsResult<block_id_t> result = this->block_allocator_->allocate();
    if (result.is_ok())
    {
      block_id_t block_id = result.unwrap();
      u64 version_id = block_id / (this->block_allocator_->bm->block_size() / sizeof(version_t));
      u64 version_offset = (block_id % (this->block_allocator_->bm->block_size() / sizeof(version_t))) * sizeof(version_t);
      // read the version
      std::vector<u8> buffer(this->block_allocator_->bm->block_size());
      this->block_allocator_->bm->read_block(version_id, buffer.data());
      // update the version
      version_t *verison_ptr = reinterpret_cast<version_t *>(buffer.data() + version_offset);
      *verison_ptr = *verison_ptr + 1;
      // write the version back
      this->block_allocator_->bm->write_partial_block(version_id, reinterpret_cast<u8 *>(verison_ptr), version_offset, sizeof(version_t));
      // return the block id and version
      std::pair<block_id_t, version_t> res(block_id, (*verison_ptr));
      return res;
    }
    else
    {
      // if the allocation failed:
      return std::pair<block_id_t, version_t>(0, 0); // return 0, 0
    }
  }

  // {Your code here}
  auto DataServer::free_block(block_id_t block_id) -> bool
  {
    // TODO: Implement this function.

    // deallocate the block
    ChfsNullResult result = this->block_allocator_->deallocate(block_id);
    if (result.is_ok())
    {
      u64 version_id = block_id / (this->block_allocator_->bm->block_size() / sizeof(version_t));
      u64 version_offset = (block_id % (this->block_allocator_->bm->block_size() / sizeof(version_t))) * sizeof(version_t);
      std::vector<u8> buffer(this->block_allocator_->bm->block_size());
      // read the version
      this->block_allocator_->bm->read_block(version_id, buffer.data());
      // update the version
      version_t *verison_ptr = reinterpret_cast<version_t *>(buffer.data() + version_offset);
      *verison_ptr = *verison_ptr + 1;
      // write the version back
      this->block_allocator_->bm->write_partial_block(version_id, reinterpret_cast<u8 *>(verison_ptr), version_offset, sizeof(version_t));
      return true;
    }
    else
      return false;
  }
} // namespace chfs