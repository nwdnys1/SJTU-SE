#include <algorithm>

#include "common/bitmap.h"
#include "distributed/commit_log.h"
#include "distributed/metadata_server.h"
#include "filesystem/directory_op.h"
#include "metadata/inode.h"
#include <chrono>

namespace chfs
{
  /**
   * `CommitLog` part
   */
  // {Your code here}
  CommitLog::CommitLog(std::shared_ptr<BlockManager> bm,
                       bool is_checkpoint_enabled)
      : is_checkpoint_enabled_(is_checkpoint_enabled), bm_(bm), log_entry_num(0) {}

  CommitLog::~CommitLog() {}

  // {Your code here}
  auto CommitLog::get_log_entry_num() -> usize
  {
    // TODO: Implement this function.
    return this->log_entry_num;
  }

  // {Your code here}
  auto CommitLog::append_log(txn_id_t txn_id, std::vector<std::shared_ptr<BlockOperation>> ops) -> void
  {
    // TODO: Implement this function.
  }

  // {Your code here}
  auto CommitLog::commit_log(txn_id_t txn_id) -> void
  {
    // TODO: Implement this function.
  }

  // {Your code here}
  auto CommitLog::checkpoint() -> void
  {
    // TODO: Implement this function.
    this->log_entry_num = 0; // reset the log entry number
  }

  // {Your code here}
  auto CommitLog::recover() -> void
  {
    // TODO: Implement this function.
  }
}; // namespace chfs