#include "fine/lock_manager.h"
#include <iostream>
#include <mutex>
#include <vector>

namespace fine {

#define LOCATE(i, len) ((i) % len)

class Storage {
public:
  Storage(LockManager *lock_manager, const std::vector<uint32_t> vectors)
      : lock_manager_(lock_manager), vector_(vectors) {
    size_ = vector_.size();
  }

  void Get(Transaction *txn, int i, std::vector<uint32_t> *v) {
    for (int k = i; k <= i; k++) {
      lock_manager_->LockShared(txn, LOCATE(i, size_));
    }

    for (int k = i; k <= i; k++) {
      v->push_back(vector_[LOCATE(i, size_)]);
    }

    for (int k = i; k <= i; k++) {
      lock_manager_->Unlock(txn, LOCATE(i, size_));
    }
  }

  void Put(Transaction *txn, int j, int n) {
    lock_manager_->LockExclusive(txn, LOCATE(j, size_));
    vector_[LOCATE(j, size_)] = n;
    lock_manager_->Unlock(txn, LOCATE(j, size_));
  }

private:
  std::vector<uint32_t> vector_;
  std::size_t size_;

  LockManager *lock_manager_{nullptr};
};
} // namespace fine
