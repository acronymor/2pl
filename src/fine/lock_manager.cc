#include "fine/lock_manager.h"
#include <thread>

namespace fine {

bool LockManager::LockShared(Transaction* txn, row_id_t row_id) {
  std::unique_lock<std::mutex> lock_guard(mutex_);

  if (txn->GetState() == TransactionState::ABORTED) {
    return false;
  }

  Request request = {txn->GetId(), LockMode::SHARED, false};
  if (lock_table_.count(row_id) == 0) {
    lock_table_[row_id].list.push_back(request);
    lock_table_[row_id].oldest = txn->GetId();
  } else {
    // 处理 deadlock, 如果发现旧的事务已经持有了锁，那么将自己杀死。
    if (lock_table_[row_id].exclusive_cnt != 0 && txn->GetId() > lock_table_[row_id].oldest) {
      txn->SetState(TransactionState::ABORTED);
      return false;
    } else {
      lock_table_[row_id].list.push_back(request);
      lock_table_[row_id].oldest = txn->GetId();
    }
  }

  std::cout << "1. LockManager::LockShared(" << txn->GetId() << "," << row_id << ")-" << std::this_thread::get_id() << std::endl;

  Request* cur;
  cond_.wait(lock_guard, [&]() -> bool {
    for (auto itr = lock_table_[row_id].list.begin(); itr != lock_table_[row_id].list.end(); itr++) {
      if (itr->txn_id != txn->GetId()) {
        if (itr->lock_mode != LockMode::SHARED || itr->granted) {
          return false;
        }
      } else {
        cur = &(*itr);
        break;
      }
    }

    return true;
  });
  std::cout << "2. LockManager::LockShared(" << txn->GetId() << "," << row_id << ")-" << std::this_thread::get_id() << std::endl;

  cur->granted = true;
  txn->GetSharedLockSet()->insert(row_id);

  cond_.notify_all();

  return true;
}

bool LockManager::LockExclusive(Transaction* txn, row_id_t row_id) {
  std::unique_lock<std::mutex> lock_guard(mutex_);

  if (txn->GetState() == TransactionState::ABORTED) {
    return false;
  }

  Request request = {txn->GetId(), LockMode::EXCLUSIVE, false};
  if (lock_table_.count(row_id) == 0) {
    lock_table_[row_id].list.push_back(request);
    lock_table_[row_id].oldest = txn->GetId();
  } else {
    // 处理 deadlock, 如果发现旧的事务已经持有了锁，那么将自己杀死。
    if (txn->GetId() > lock_table_[row_id].oldest) {
      txn->SetState(TransactionState::ABORTED);
      return false;
    } else {
      lock_table_[row_id].list.push_back(request);
      lock_table_[row_id].oldest = txn->GetId();
    }
  }

  std::cout << "1. LockManager::LockExclusive(" << txn->GetId() << "," << row_id << ")-" << std::this_thread::get_id() << std::endl;
  Request* cur;
  cond_.wait(lock_guard, [&]() -> bool {
    for (auto itr = lock_table_[row_id].list.begin(); itr != lock_table_[row_id].list.end(); itr++) {
      if (itr->txn_id != txn->GetId()) {
        if (itr->granted) {
          return false;
        }
      } else {
        cur = &(*itr);
        break;
      }
    }
    std::cout << "2. LockManager::LockExclusive(" << txn->GetId() << "," << row_id << ")-" << std::this_thread::get_id() << std::endl;

    return true;
  });

  cur->granted = true;
  txn->GetExclusiveLockSet()->insert(row_id);

  return true;
}
bool LockManager::LockUpgrade(Transaction* txn, row_id_t row_id) {
  std::unique_lock<std::mutex> lock_guard(mutex_);

  if (txn->GetState() == TransactionState::ABORTED) {
    return false;
  }

  cond_.wait(lock_guard, [&]() -> bool {
    for (auto it = lock_table_[row_id].list.begin(); it != lock_table_[row_id].list.end(); ++it) {
      if (it == lock_table_[row_id].list.begin() && it->txn_id != txn->GetId()) {
        return false;
      }

      if (it != lock_table_[row_id].list.begin() && it->granted) {
        return false;
      }
    }

    lock_table_[row_id].list.begin()->lock_mode = LockMode::EXCLUSIVE;
    lock_table_[row_id].exclusive_cnt++;
    txn->GetSharedLockSet()->erase(row_id);
    txn->GetExclusiveLockSet()->insert(row_id);

    return true;
  });

  return true;
}

bool LockManager::Unlock(Transaction* txn, row_id_t row_id) {
  std::unique_lock<std::mutex> lock_guard(mutex_);

  // strict 2pl
  /*
  if (txn->GetState() != TransactionState::ABORTED || txn->GetState() != TransactionState::COMMITTED) {
    txn->SetState(TransactionState::ABORTED);
    return false;
  }
   */

  if (txn->GetState() == TransactionState::GROWING) {
    txn->SetState(TransactionState::SHRINKING);
  }

  std::cout << "1. LockManager::Unlock(" << txn->GetId() << "," << row_id << ")-" << std::this_thread::get_id() << std::endl;
  for (auto it = lock_table_[row_id].list.begin(); it != lock_table_[row_id].list.end(); ++it) {
    if (it->txn_id == txn->GetId()) {
      if (it->lock_mode == LockMode::SHARED) {
        txn->GetSharedLockSet()->erase(row_id);
      } else {
        txn->GetExclusiveLockSet()->erase(row_id);
        lock_table_[row_id].exclusive_cnt--;
      }
      lock_table_[row_id].list.erase(it);
      break;
    }
  }

  for (auto it = lock_table_[row_id].list.begin(); it != lock_table_[row_id].list.end(); ++it) {
    if (it->txn_id < lock_table_[row_id].oldest) {
      lock_table_[row_id].oldest = it->txn_id;
    }
  }
  std::cout << "2. LockManager::Unlock(" << txn->GetId() << "," << row_id << ")-" << std::this_thread::get_id() << std::endl;

  cond_.notify_all();

  return true;
}
}// namespace fine
