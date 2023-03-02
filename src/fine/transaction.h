#pragma once

#include <iostream>
#include <memory>
#include <unordered_set>

namespace fine {
typedef uint32_t txn_id_t;
typedef uint64_t row_id_t;

enum class TransactionState { GROWING,
                              SHRINKING,
                              COMMITTED,
                              ABORTED };

class Transaction {
 public:
  Transaction(txn_id_t txn_id) : txn_id_(txn_id) {
    shared_lock_set_ = std::make_shared<std::unordered_set<row_id_t>>();
    exclusive_lock_set_ = std::make_shared<std::unordered_set<row_id_t>>();
  }

 public:
  inline std::shared_ptr<std::unordered_set<row_id_t>> GetSharedLockSet() {
    return shared_lock_set_;
  }

  inline txn_id_t GetId() const { return txn_id_; }

  inline std::shared_ptr<std::unordered_set<row_id_t>> GetExclusiveLockSet() {
    return exclusive_lock_set_;
  }

  inline TransactionState GetState() { return state_; }

  inline void SetState(TransactionState state) { state_ = state; }

 private:
  std::shared_ptr<std::unordered_set<row_id_t>> shared_lock_set_;
  std::shared_ptr<std::unordered_set<row_id_t>> exclusive_lock_set_;

  txn_id_t txn_id_;

  TransactionState state_;
};
}// namespace fine
