#include "distributed/client.h"
#include "common/macros.h"
#include "common/util.h"
#include "distributed/metadata_server.h"

namespace chfs
{

  ChfsClient::ChfsClient() : num_data_servers(0) {}

  auto ChfsClient::reg_server(ServerType type, const std::string &address,
                              u16 port, bool reliable) -> ChfsNullResult
  {
    switch (type)
    {
    case ServerType::DATA_SERVER:
      num_data_servers += 1;
      data_servers_.insert({num_data_servers, std::make_shared<RpcClient>(
                                                  address, port, reliable)});
      break;
    case ServerType::METADATA_SERVER:
      metadata_server_ = std::make_shared<RpcClient>(address, port, reliable);
      break;
    default:
      std::cerr << "Unknown Type" << std::endl;
      exit(1);
    }

    return KNullOk;
  }

  // {Your code here}
  auto ChfsClient::mknode(FileType type, inode_id_t parent, const std::string &name) -> ChfsResult<inode_id_t>
  {
    // TODO: Implement this function.
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("mknode", (u8)type, parent, name);
    if (rpc_res.is_err()) // if error
      return ChfsResult<inode_id_t>(rpc_res.unwrap_error());

    inode_id_t inode_id = rpc_res.unwrap()->as<inode_id_t>();
    if (inode_id == 0)
      return ChfsResult<inode_id_t>(ErrorType::OUT_OF_RESOURCE);

    return ChfsResult<inode_id_t>(inode_id);
  }

  // {Your code here}
  auto ChfsClient::unlink(inode_id_t parent, std::string const &name) -> ChfsNullResult
  {
    // TODO: Implement this function.
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("unlink", parent, name);
    if (rpc_res.is_err())
      return ChfsNullResult(rpc_res.unwrap_error());
    // if success
    if (rpc_res.unwrap()->as<bool>())
      return KNullOk;
    else
      return ChfsNullResult(ErrorType::DONE);
  }

  // {Your code here}
  auto ChfsClient::lookup(inode_id_t parent, const std::string &name)
      -> ChfsResult<inode_id_t>
  {
    // TODO: Implement this function.
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("lookup", parent, name);
    if (rpc_res.is_err())
      return ChfsResult<inode_id_t>(rpc_res.unwrap_error());
    inode_id_t inode_id = rpc_res.unwrap()->as<inode_id_t>();
    if (inode_id == 0)
      return ChfsResult<inode_id_t>(ErrorType::NotExist);
    else
      return ChfsResult<inode_id_t>(inode_id);
  }

  // {Your code here}
  auto ChfsClient::readdir(inode_id_t id)
      -> ChfsResult<std::vector<std::pair<std::string, inode_id_t>>>
  {
    // TODO: Implement this function.
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("readdir", id);
    if (rpc_res.is_err())
      return ChfsResult<std::vector<std::pair<std::string, inode_id_t>>>(rpc_res.unwrap_error());
    return ChfsResult<std::vector<std::pair<std::string, inode_id_t>>>(rpc_res.unwrap()->as<std::vector<std::pair<std::string, inode_id_t>>>());
  }

  // {Your code here}
  auto ChfsClient::get_type_attr(inode_id_t id)
      -> ChfsResult<std::pair<InodeType, FileAttr>>
  {
    // TODO: Implement this function.
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("get_type_attr", id);
    if (rpc_res.is_err())
      return ChfsResult<std::pair<InodeType, FileAttr>>(rpc_res.unwrap_error());
    std::tuple<u64, u64, u64, u64, u8> type_attr = rpc_res.unwrap()->as<std::tuple<u64, u64, u64, u64, u8>>();
    InodeType inode_type;
    FileAttr file_attr;
    // if the type is regular file
    if (std::get<4>(type_attr) == (u8)FileType::REGULAR)
      inode_type = InodeType::FILE;
    else // if the type is directory
      inode_type = InodeType::Directory;
    // set the attribute
    file_attr.size = std::get<0>(type_attr);
    file_attr.atime = std::get<1>(type_attr);
    file_attr.ctime = std::get<2>(type_attr);
    file_attr.mtime = std::get<3>(type_attr);
    return ChfsResult<std::pair<InodeType, FileAttr>>(std::pair<InodeType, FileAttr>(inode_type, file_attr));
  }

  /**
   * Read and Write operations are more complicated.
   */
  // {Your code here}
  auto ChfsClient::read_file(inode_id_t id, usize offset, usize size)
      -> ChfsResult<std::vector<u8>>
  {
    // TODO: Implement this function.
    if (size == 0)
      return ChfsResult<std::vector<u8>>(std::vector<u8>());
    std::vector<u8> res;
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("get_block_map", id);
    if (rpc_res.is_err())
      return ChfsResult<std::vector<u8>>(rpc_res.unwrap_error());

    std::vector<BlockInfo> block_map = rpc_res.unwrap()->as<std::vector<BlockInfo>>();
    // check the offset and size
    if (offset + size - 1 >= block_map.size() * DiskBlockSize)
      return ChfsResult<std::vector<u8>>(ErrorType::INVALID_ARG);

    u32 start_block = offset / DiskBlockSize;
    u32 end_block = (offset + size - 1) / DiskBlockSize;

    for (int i = start_block; i <= end_block; ++i)
    {
      // get the block id, machine id and version
      auto block_id = std::get<0>(block_map[i]);
      auto machine_id = std::get<1>(block_map[i]);
      auto version = std::get<2>(block_map[i]);
      // get the start and end bit of the block
      u32 start_bit = 0;
      u32 end_bit = DiskBlockSize - 1;

      if (i == start_block) // for the first block, set the start bit
        start_bit = offset % DiskBlockSize;
      if (i == end_block) // for the last block, set the end bit
        end_bit = (offset + size - 1) % DiskBlockSize;
      // use rpc to call data server
      auto rpc_res = (this->data_servers_[machine_id])->call("read_data", block_id, start_bit, end_bit - start_bit + 1, version);
      if (rpc_res.is_err())
        return ChfsResult<std::vector<u8>>(rpc_res.unwrap_error());
      std::vector<u8> file_data = rpc_res.unwrap()->as<std::vector<u8>>();
      if (file_data.size() == 0)
        return ChfsResult<std::vector<u8>>(ErrorType::INVALID);
      // insert the data into res
      res.insert(res.end(), file_data.begin(), file_data.end());
    }

    return ChfsResult<std::vector<u8>>(res);
  }

  // {Your code here}
  auto ChfsClient::write_file(inode_id_t id, usize offset, std::vector<u8> data)
      -> ChfsNullResult
  {
    // TODO: Implement this function.
    if (data.empty())
      return KNullOk;
    // get the type and attribute
    auto type_attr = this->get_type_attr(id);
    if (type_attr.is_err())
      return ChfsNullResult(type_attr.unwrap_error());

    auto old_size = type_attr.unwrap().second.size;
    auto size = data.size();
    // allocate enough block for the write
    while (offset + size - 1 >= old_size)
    {
      // use rpc to call metadata server
      auto rpc_res = this->metadata_server_->call("alloc_block", id);
      if (rpc_res.is_err())
        return ChfsNullResult(rpc_res.unwrap_error());
      old_size += DiskBlockSize;
    }

    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("get_block_map", id);
    if (rpc_res.is_err())
      return ChfsNullResult(rpc_res.unwrap_error());
    std::vector<BlockInfo> block_map = rpc_res.unwrap()->as<std::vector<BlockInfo>>();

    // write the data into the block
    u32 start_block = offset / DiskBlockSize;
    u32 end_block = (offset + size - 1) / DiskBlockSize;
    u64 cnt = 0; // count the bytes that have been written
    for (auto i = start_block; i <= end_block; ++i)
    {
      auto block_id = std::get<0>(block_map[i]);
      auto machine_id = std::get<1>(block_map[i]);
      // get the start and end bit of the block
      u32 start_bit = 0;
      u32 end_bit = DiskBlockSize - 1;
      if (i == start_block)
        start_bit = offset % DiskBlockSize;
      if (i == end_block)
        end_bit = (offset + size - 1) % DiskBlockSize;
      // get the data that need to be written
      std::vector<u8> buffer;
      buffer.insert(buffer.end(), data.begin() + cnt, data.begin() + cnt + end_bit - start_bit + 1);
      // use rpc to call data server
      auto rpc_res = (this->data_servers_[machine_id])->call("write_data", block_id, start_bit, buffer);
      if (rpc_res.is_err())
        return ChfsNullResult(rpc_res.unwrap_error());
      if (!rpc_res.unwrap()->as<bool>())
        return ChfsNullResult(ErrorType::DONE);
      cnt += end_bit - start_bit + 1;
    }
    return KNullOk;
  }

  // {Your code here}
  auto ChfsClient::free_file_block(inode_id_t id, block_id_t block_id, mac_id_t mac_id) -> ChfsNullResult
  {
    // TODO: Implement this function.
    // use rpc to call metadata server
    auto rpc_res = this->metadata_server_->call("free_block", id, block_id, mac_id);
    if (rpc_res.is_err())
      return ChfsNullResult(rpc_res.unwrap_error());

    if (rpc_res.unwrap()->as<bool>())
      return KNullOk;
    else
      return ChfsNullResult(ErrorType::DONE);
  }

} // namespace chfs