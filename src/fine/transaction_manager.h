#include "fine/lock_manager.h"
#include "fine/transaction.h"
#include <atomic>

namespace fine {
class TransactionManager {
 public:
  TransactionManager(LockManager* lock_manager) : next_txn_id_(0), lock_manager_(lock_manager) {}

  Transaction* Begin();
  void Commit(Transaction* txn);
  void Abort(Transaction* txn);

 private:
  std::atomic<txn_id_t> next_txn_id_;
  LockManager* lock_manager_;
};

}// namespace fine
