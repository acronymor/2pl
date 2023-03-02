#include <stdlib.h>
#include <time.h>

#include <thread>

#include "comm/config.h"
#include "fine/fine_storage.h"
#include "fine/transaction_manager.h"
#include "gtest/gtest.h"

/**
 * 采用乐观策略，对 vector 指定 index 加锁。
 */

namespace fine {

class TwoPlTest : public ::testing::Test {
 protected:
  void SetUp() override {
    for (uint32_t i = 0; i < kMaxValue; i++) {
      vector.push_back(i);
    }

    lock_manager_ = new LockManager();
    txn_manager_ = new TransactionManager(lock_manager_);
    exec = new Storage(lock_manager_, vector);

    srand(time(nullptr));
  }

  void TearDown() override {
    delete lock_manager_;
    delete txn_manager_;
    delete exec;
  }

 protected:
  Storage* exec{nullptr};
  LockManager* lock_manager_;
  TransactionManager* txn_manager_;
  std::vector<uint32_t> vector;
};

TEST_F(TwoPlTest, DISABLED_oneWorker) {
  int i = 0, j = 3;

  Transaction* txn = txn_manager_->Begin();
  exec->Put(txn, i, j);
  txn_manager_->Commit(txn);

  ASSERT_EQ(0, exec->Get(0));
  ASSERT_EQ(1, exec->Get(1));
  ASSERT_EQ(2, exec->Get(2));
  ASSERT_EQ(3, exec->Get(3));
}

TEST_F(TwoPlTest, twoWorker) {
  std::thread worker1([&]() -> void {
    Transaction* txn = txn_manager_->Begin();
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(txn, i, j);
    }
    txn_manager_->Commit(txn);
  });

  std::thread worker2([&]() -> void {
    Transaction* txn = txn_manager_->Begin();
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(txn, i, j);
    }
    txn_manager_->Commit(txn);
  });

  worker1.join();
  worker2.join();
}

TEST_F(TwoPlTest, DISABLED_threeWorker) {
  std::thread worker1([&]() -> void {
    Transaction* txn = txn_manager_->Begin();
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(txn, i, j);
    }
    txn_manager_->Commit(txn);
  });

  std::thread worker2([&]() -> void {
    Transaction* txn = txn_manager_->Begin();
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(txn, i, j);
    }
    txn_manager_->Commit(txn);
  });

  worker1.join();
  worker2.join();
}
}  // namespace fine
