#pragma once

#include "fine/transaction.h"
#include <condition_variable>
#include <iostream>
#include <list>
#include <mutex>
#include <unordered_map>

namespace fine {
enum class LockMode { SHARED = 0,
                      EXCLUSIVE };

class LockManager {
 public:
  struct Request {
    Request(txn_id_t id, LockMode m, bool g) : txn_id(id), lock_mode(m), granted(g) {}

    txn_id_t txn_id;
    LockMode lock_mode;
    bool granted;
  };

  struct WaitList {
    int exclusive_cnt = 0;
    txn_id_t oldest = -1;
    std::list<Request> list;
  };

 public:
  bool LockShared(Transaction* txn, row_id_t row_id);
  bool LockExclusive(Transaction* txn, row_id_t row_id);
  bool LockUpgrade(Transaction* txn, row_id_t row_id);
  bool Unlock(Transaction* txn, row_id_t row_id);

 private:
  std::mutex mutex_;
  std::condition_variable cond_;

  std::unordered_map<row_id_t, WaitList> lock_table_;
};

}// namespace fine
