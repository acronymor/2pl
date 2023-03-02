#include "fine/lock_manager.h"
#include <iostream>
#include <mutex>
#include <vector>

namespace fine {

#define LOCATE(i, len) ((i) % len)

class Storage {
 public:
  Storage(LockManager* lock_manager, const std::vector<uint32_t> vectors) : lock_manager_(lock_manager), vector_(vectors) {
    size_ = vector_.size();
  }

  void Put(Transaction* txn, int i, int j) {
    lock_manager_->LockShared(txn, LOCATE(i, size_));
    lock_manager_->LockShared(txn, LOCATE(i + 1, size_));
    lock_manager_->LockShared(txn, LOCATE(i + 2, size_));

    int a = vector_[LOCATE(i, size_)];
    int b = vector_[LOCATE(i + 1, size_)];
    int c = vector_[LOCATE(i + 2, size_)];
    int d = a + b + c;

    lock_manager_->LockExclusive(txn, LOCATE(j, size_));
    vector_[LOCATE(j, size_)] = d;
    lock_manager_->Unlock(txn, LOCATE(j, size_));

    std::cout << i << "-" << a << "," << i + 1 << "-" << b << "," << i + 2 << "-" << c << ",d-" << d;
    std::cout << "," << j << "-" << vector_[LOCATE(j, size_)] << std::endl;

    lock_manager_->Unlock(txn, LOCATE(i, size_));
    lock_manager_->Unlock(txn, LOCATE(i + 1, size_));
    lock_manager_->Unlock(txn, LOCATE(i + 2, size_));
  }

  // 仅仅用于测试结果，不可以做其他用途，且未保证线程安全
  int Get(int i) {
    return vector_[LOCATE(i, size_)];
  }

 private:
  std::vector<uint32_t> vector_;
  std::size_t size_;

  LockManager* lock_manager_{nullptr};
};
}// namespace fine
