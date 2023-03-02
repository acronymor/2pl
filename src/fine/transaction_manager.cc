#include "fine/transaction_manager.h"

namespace fine {

Transaction* TransactionManager::Begin() {
  Transaction* txn = new Transaction(next_txn_id_++);
  return txn;
}

void TransactionManager::Commit(Transaction* txn) {
  txn->SetState(TransactionState::COMMITTED);

  std::unordered_set<row_id_t> lock_set;
  for (auto item : *txn->GetSharedLockSet()) {
    lock_set.emplace(item);
  }
  for (auto item : *txn->GetExclusiveLockSet()) {
    lock_set.emplace(item);
  }

  for (auto locked_rid : lock_set) {
    lock_manager_->Unlock(txn, locked_rid);
  }
}

void TransactionManager::Abort(Transaction* txn) {
  txn->SetState(TransactionState::ABORTED);

  std::unordered_set<row_id_t> lock_set;
  for (auto item : *txn->GetSharedLockSet()) {
    lock_set.emplace(item);
  }
  for (auto item : *txn->GetExclusiveLockSet()) {
    lock_set.emplace(item);
  }

  for (auto locked_rid : lock_set) {
    lock_manager_->Unlock(txn, locked_rid);
  }
}

}// namespace fine
