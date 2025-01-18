#include <ctime>

#include "filesystem/operations.h"
#include "filesystem/directory_op.h"

namespace chfs
{

  // {Your code here}
  auto FileOperation::alloc_inode(InodeType type) -> ChfsResult<inode_id_t>
  {

    // TODO:
    // 1. Allocate a block for the inode.
    // 2. Allocate an inode.
    // 3. Initialize the inode block
    //    and write the block back to block manager.
    ChfsResult<block_id_t> block_id = block_allocator_->allocate(); // allocate a block for the inode
    if (block_id.is_err())
      return ChfsResult<inode_id_t>(ErrorType::OUT_OF_RESOURCE);

    ChfsResult<inode_id_t> inode_id = inode_manager_->allocate_inode(type, block_id.unwrap()); // allocate an inode,this function will initialize the inode block and write the block back to block manager
    if (inode_id.is_err())
      return ChfsResult<inode_id_t>(ErrorType::OUT_OF_RESOURCE);

    auto inode_res = inode_id.unwrap();

    return inode_res;
  }

  auto FileOperation::getattr(inode_id_t id) -> ChfsResult<FileAttr>
  {
    return this->inode_manager_->get_attr(id);
  }

  auto FileOperation::get_type_attr(inode_id_t id)
      -> ChfsResult<std::pair<InodeType, FileAttr>>
  {
    return this->inode_manager_->get_type_attr(id);
  }

  auto FileOperation::gettype(inode_id_t id) -> ChfsResult<InodeType>
  {
    return this->inode_manager_->get_type(id);
  }

  auto calculate_block_sz(u64 file_sz, u64 block_sz) -> u64
  {
    return (file_sz % block_sz) ? (file_sz / block_sz + 1) : (file_sz / block_sz);
  }

  auto FileOperation::write_file_w_off(inode_id_t id, const char *data, u64 sz,
                                       u64 offset) -> ChfsResult<u64>
  {
    auto read_res = this->read_file(id);
    if (read_res.is_err())
    {
      return ChfsResult<u64>(read_res.unwrap_error());
    }

    auto content = read_res.unwrap();
    if (offset + sz > content.size())
    {
      content.resize(offset + sz);
    }
    memcpy(content.data() + offset, data, sz);

    auto write_res = this->write_file(id, content);
    if (write_res.is_err())
    {
      return ChfsResult<u64>(write_res.unwrap_error());
    }
    return ChfsResult<u64>(sz);
  }

  // {Your code here}
  auto FileOperation::write_file(inode_id_t id, const std::vector<u8> &content)
      -> ChfsNullResult
  {
    auto error_code = ErrorType::DONE;
    const auto block_size = this->block_manager_->block_size();
    usize old_block_num = 0;
    usize new_block_num = 0;
    u64 original_file_sz = 0;

    // 1. read the inode
    std::vector<u8> inode(block_size);
    std::vector<u8> indirect_block(0);
    indirect_block.reserve(block_size);

    auto inode_p = reinterpret_cast<Inode *>(inode.data());
    auto inlined_blocks_num = 0;

    auto inode_res = this->inode_manager_->read_inode(id, inode);
    if (inode_res.is_err())
    {
      error_code = inode_res.unwrap_error();
      // I know goto is bad, but we have no choice
      goto err_ret;
    }
    else
    {
      inlined_blocks_num = inode_p->get_direct_block_num();
    }

    if (content.size() > inode_p->max_file_sz_supported())
    {
      std::cerr << "file size too large: " << content.size() << " vs. "
                << inode_p->max_file_sz_supported() << std::endl;
      error_code = ErrorType::OUT_OF_RESOURCE;
      goto err_ret;
    }

    // 2. make sure whether we need to allocate more blocks
    original_file_sz = inode_p->get_size();
    old_block_num = calculate_block_sz(original_file_sz, block_size);
    new_block_num = calculate_block_sz(content.size(), block_size);

    if (new_block_num > old_block_num)
    {
      // If we need to allocate more blocks.
      for (usize idx = old_block_num; idx < new_block_num; ++idx)
      {

        // TODO: Implement the case of allocating more blocks.
        // 1. Allocate a block.
        // 2. Fill the allocated block id to the inode.
        //    You should pay attention to the case of indirect block.
        //    You may use function `get_or_insert_indirect_block`
        //    in the case of indirect block.
        ChfsResult<block_id_t> block_id = block_allocator_->allocate();
        if (block_id.is_err())
          return ChfsNullResult(ErrorType::OUT_OF_RESOURCE);

        if (inode_p->is_direct_block(idx)) // direct block
          inode_p->blocks[idx] = block_id.unwrap();
        else // indirect block
        {
          ChfsResult<block_id_t> indirect_block_id = inode_p->get_or_insert_indirect_block(block_allocator_);
          if (indirect_block_id.is_err())
            return ChfsNullResult(ErrorType::OUT_OF_RESOURCE);

          block_id_t new_block_id = block_id.unwrap();
          // write the new block id to the indirect block
          block_manager_->write_partial_block(indirect_block_id.unwrap(), reinterpret_cast<u8 *>(&new_block_id), sizeof(block_id_t) * (idx - inode_p->get_direct_block_num()), sizeof(block_id_t));
        }
      }
    }
    else
    {
      // We need to free the extra blocks.
      for (usize idx = new_block_num; idx < old_block_num; ++idx)
      {
        if (inode_p->is_direct_block(idx))
        {

          // TODO: Free the direct extra block.
          block_allocator_->deallocate(inode_p->blocks[idx]);
          inode_p->blocks[idx] = chfs::KInvalidBlockID;
        }
        else
        {

          // TODO: Free the indirect extra block.
          block_id_t indirect_block_id = inode_p->get_indirect_block_id();
          block_manager_->read_block(indirect_block_id, indirect_block.data());
          block_allocator_->deallocate(reinterpret_cast<block_id_t *>(indirect_block.data())[idx - inode_p->get_direct_block_num()]);

          block_id_t new_block_id = chfs::KInvalidBlockID;
          block_manager_->write_partial_block(indirect_block_id, reinterpret_cast<u8 *>(&new_block_id), sizeof(block_id_t) * (idx - inode_p->get_direct_block_num()), sizeof(block_id_t));
        }
      }

      // If there are no more indirect blocks.
      if (old_block_num > inlined_blocks_num &&
          new_block_num <= inlined_blocks_num && true)
      {

        auto res =
            this->block_allocator_->deallocate(inode_p->get_indirect_block_id());
        if (res.is_err())
        {
          error_code = res.unwrap_error();
          goto err_ret;
        }
        indirect_block.clear();
        inode_p->invalid_indirect_block_id();
      }
    }

    // 3. write the contents
    inode_p->inner_attr.size = content.size();
    inode_p->inner_attr.mtime = time(0);

    {
      auto block_idx = 0;
      u64 write_sz = 0;
      block_id_t block_id = 0;

      while (write_sz < content.size())
      {
        auto sz = ((content.size() - write_sz) > block_size)
                      ? block_size
                      : (content.size() - write_sz);
        std::vector<u8> buffer(block_size);
        memcpy(buffer.data(), content.data() + write_sz, sz);

        if (inode_p->is_direct_block(block_idx))
        {

          // TODO: Implement getting block id of current direct block.
          block_id = inode_p->blocks[block_idx];
        }
        else
        {

          // TODO: Implement getting block id of current indirect block.
          block_manager_->read_block(inode_p->get_indirect_block_id(), indirect_block.data());                           // read the indirect block to get the block id
          block_id = reinterpret_cast<block_id_t *>(indirect_block.data())[block_idx - inode_p->get_direct_block_num()]; // convert the indirect block to block id
        }

        // TODO: Write to current block.
        block_manager_->write_block(block_id, buffer.data());

        write_sz += sz;
        block_idx += 1;
      }
    }

    // finally, update the inode
    {
      inode_p->inner_attr.set_all_time(time(0));

      auto write_res =
          this->block_manager_->write_block(inode_res.unwrap(), inode.data());
      if (write_res.is_err())
      {
        error_code = write_res.unwrap_error();
        goto err_ret;
      }
      if (indirect_block.size() != 0)
      {
        write_res =
            inode_p->write_indirect_block(this->block_manager_, indirect_block);
        if (write_res.is_err())
        {
          error_code = write_res.unwrap_error();
          goto err_ret;
        }
      }
    }

    return KNullOk;

  err_ret:
    // std::cerr << "write file return error: " << (int)error_code << std::endl;
    return ChfsNullResult(error_code);
  }

  // {Your code here}
  auto FileOperation::read_file(inode_id_t id) -> ChfsResult<std::vector<u8>>
  {
    auto error_code = ErrorType::DONE;
    std::vector<u8> content;

    const auto block_size = this->block_manager_->block_size();

    // 1. read the inode
    std::vector<u8> inode(block_size);
    std::vector<u8> indirect_block(0);
    indirect_block.reserve(block_size);

    auto inode_p = reinterpret_cast<Inode *>(inode.data());
    u64 file_sz = 0;
    u64 read_sz = 0;

    auto inode_res = this->inode_manager_->read_inode(id, inode);
    if (inode_res.is_err())
    {
      error_code = inode_res.unwrap_error();
      // I know goto is bad, but we have no choice
      goto err_ret;
    }

    file_sz = inode_p->get_size();
    content.reserve(file_sz);

    // Now read the file
    while (read_sz < file_sz)
    {
      auto sz = ((inode_p->get_size() - read_sz) > block_size)
                    ? block_size
                    : (inode_p->get_size() - read_sz);
      std::vector<u8> buffer(block_size);

      // Get current block id.
      if (inode_p->is_direct_block(read_sz / block_size))
      {
        // TODO: Implement the case of direct block.
        block_id_t block_id = inode_p->blocks[read_sz / block_size];
        block_manager_->read_block(block_id, buffer.data());
      }
      else
      {
        // TODO: Implement the case of indirect block.
        block_manager_->read_block(inode_p->get_indirect_block_id(), indirect_block.data());
        block_id_t block_id = reinterpret_cast<block_id_t *>(indirect_block.data())[(read_sz / block_size) - inode_p->get_direct_block_num()];
        block_manager_->read_block(block_id, buffer.data());
      }

      // TODO: Read from current block and store to `content`.
      for (u64 i = 0; i < sz; ++i)
        content.push_back(buffer[i]);

      read_sz += sz;
    }

    return ChfsResult<std::vector<u8>>(std::move(content));

  err_ret:
    return ChfsResult<std::vector<u8>>(error_code);
  }

  auto FileOperation::read_file_w_off(inode_id_t id, u64 sz, u64 offset)
      -> ChfsResult<std::vector<u8>>
  {
    auto res = read_file(id);
    if (res.is_err())
    {
      return res;
    }

    auto content = res.unwrap();
    return ChfsResult<std::vector<u8>>(
        std::vector<u8>(content.begin() + offset, content.begin() + offset + sz));
  }

  auto FileOperation::resize(inode_id_t id, u64 sz) -> ChfsResult<FileAttr>
  {
    auto attr_res = this->getattr(id);
    if (attr_res.is_err())
    {
      return ChfsResult<FileAttr>(attr_res.unwrap_error());
    }

    auto attr = attr_res.unwrap();
    auto file_content = this->read_file(id);
    if (file_content.is_err())
    {
      return ChfsResult<FileAttr>(file_content.unwrap_error());
    }

    auto content = file_content.unwrap();

    if (content.size() != sz)
    {
      content.resize(sz);

      auto write_res = this->write_file(id, content);
      if (write_res.is_err())
      {
        return ChfsResult<FileAttr>(write_res.unwrap_error());
      }
    }

    attr.size = sz;
    return ChfsResult<FileAttr>(attr);
  }

  // add a blockinfo to a regular file
  ChfsNullResult FileOperation::add_blockinfo(inode_id_t inode_id, BlockInfo &block_info)
  {
    const chfs::usize block_size = this->block_manager_->block_size();
    block_id_t block_id = 0;

    // read inode
    std::vector<u8> regular_inode(block_size);
    auto inode_ptr = reinterpret_cast<Inode *>(regular_inode.data());
    ChfsResult<block_id_t> res = this->inode_manager_->read_inode(inode_id, regular_inode);
    if (res.is_err())
      return ChfsNullResult(ErrorType::NotExist);
    else
      block_id = res.unwrap();

    // add blockinfo to the inode
    u64 original_file_sz = inode_ptr->get_size();
    usize old_block_num = original_file_sz / block_size;
    usize new_block_num = old_block_num + 1;
    // check if the new block can be added
    if (new_block_num * sizeof(BlockInfo) + sizeof(Inode) > block_size)
      return ChfsNullResult(ErrorType::OUT_OF_RESOURCE);
    inode_ptr->blockinfo_list[old_block_num] = block_info;
    // update the inode
    inode_ptr->inner_attr.size = new_block_num * block_size;
    inode_ptr->inner_attr.set_all_time(time(0));
    // write back the inode
    if (this->block_manager_->write_block(block_id, regular_inode.data()).is_err())
      return ChfsNullResult(ErrorType::DONE);

    return KNullOk;
  }

  // unlink a regular file without freeing the blocks
  ChfsNullResult FileOperation::unlink_block(inode_id_t inode_id, const std::string &file_name)
  {
    std::list<DirectoryEntry> file_list;
    read_directory(this, inode_id, file_list);
    // find the file
    for (auto it = file_list.begin(); it != file_list.end(); ++it)
    {
      if (it->name == file_name)
      {
        inode_id_t inode_id = it->id;
        // deallocate the blocks
        auto inode_res = this->inode_manager_->get(inode_id);
        // if the inode doesn't exist
        if (inode_res.is_err())
          return ChfsNullResult(ErrorType::NotExist);
        // if deallocate failed
        if (this->block_allocator_->deallocate(inode_res.unwrap()).is_err())
          return ChfsNullResult(ErrorType::DONE);
        // free the inode
        if (this->inode_manager_->free_inode(inode_id).is_err())
          return ChfsNullResult(ErrorType::DONE);
        // remove the file from the directory
        file_list.erase(it);
        std::string list_string = dir_list_to_string(file_list);
        std::vector<u8> buffer(list_string.begin(), list_string.end());
        // write back the directory
        write_file(inode_id, buffer);
        return (KNullOk);
      }
    }
    // if the file doesn't exist
    return ChfsNullResult(ErrorType::NotExist);
  }

  // remove a blockinfo from a regular file
  ChfsNullResult FileOperation::remove_blockinfo(inode_id_t inode_id, block_id_t block_id, mac_id_t machine_id)
  {
    const auto block_size = this->block_manager_->block_size();
    // read inode
    std::vector<u8> inode(block_size);
    auto inode_ptr = reinterpret_cast<Inode *>(inode.data());
    auto res = this->inode_manager_->read_inode(inode_id, inode);
    if (res.is_err())
      return ChfsNullResult(ErrorType::NotExist);

    block_id_t inode_block_id = res.unwrap();
    usize block_num = inode_ptr->get_size() / block_size;

    // get blockinfo_list
    std::vector<BlockInfo> blockinfo_list;
    // find the blockinfo to remove
    for (u32 i = 0; i < block_num; ++i)
    {
      auto item_block_id = std::get<0>(inode_ptr->blockinfo_list[i]);
      auto item_mac_id = std::get<1>(inode_ptr->blockinfo_list[i]);
      // if the blockinfo is the one to remove
      if (item_block_id == block_id && item_mac_id == machine_id)
        continue;
      blockinfo_list.push_back(inode_ptr->blockinfo_list[i]);
    }
    u32 blockinfo_list_size = blockinfo_list.size();

    // update the inode
    inode_ptr->inner_attr.set_all_time(time(0));
    inode_ptr->inner_attr.size = blockinfo_list_size * block_size;
    for (u32 i = 0; i < blockinfo_list_size; ++i)
      inode_ptr->blockinfo_list[i] = blockinfo_list[i];

    // write back the inode
    if (this->block_manager_->write_block(inode_block_id, inode.data()).is_err())
      return ChfsNullResult(ErrorType::DONE);

    return KNullOk;
  }

  // get blockinfo list of a regular file
  ChfsNullResult FileOperation::get_blockinfo_list(inode_id_t inode_id, std::vector<BlockInfo> &res)
  {
    const auto block_size = this->block_manager_->block_size();
    std::vector<u8> inode(block_size);
    auto inode_ptr = reinterpret_cast<Inode *>(inode.data());
    // read inode
    if (this->inode_manager_->read_inode(inode_id, inode).is_err())
      return ChfsNullResult(ErrorType::NotExist);
    usize block_num = inode_ptr->get_size() / block_size;
    // get blockinfo_list
    for (u32 i = 0; i < block_num; ++i)
      res.push_back(inode_ptr->blockinfo_list[i]);

    return KNullOk;
  }

} // namespace chfs
