#include <stdlib.h>
#include <time.h>

#include <thread>

#include "coarse/coarse_storage.h"
#include "comm/config.h"
#include "gtest/gtest.h"

/**
 * 采用悲观策略，将整个 vector 全部锁住，这样相当于强制转为单线程操作，所以一定不会错。不过性能堪忧。
 */

namespace coarse {

class StorageTest : public ::testing::Test {
 protected:
  void SetUp() override {
    for (uint32_t i = 0; i < kMaxValue; i++) {
      vector.push_back(i);
    }
    exec = new Storage(vector);

    srand(time(nullptr));
  }

  void TearDown() override { delete exec; }

 protected:
  Storage* exec{nullptr};
  std::vector<uint32_t> vector;
};

TEST_F(StorageTest, DISABLED_oneWorker) {
  int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
  exec->Put(i, j);
}

TEST_F(StorageTest, twoWorker) {
  std::thread worker1([&]() -> void {
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(i, j);
    }
  });

  std::thread worker2([&]() -> void {
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(i, j);
    }
  });

  worker1.join();
  worker2.join();
}

TEST_F(StorageTest, DISABLED_threeWorker) {
  std::thread worker1([&]() -> void {
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(i, j);
    }
  });

  std::thread worker2([&]() -> void {
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(i, j);
    }
  });

  std::thread worker3([&]() -> void {
    for (int k = 0; k < kOptCnt; k++) {
      int i = RANDOM(kMinValue, kMaxValue), j = RANDOM(kMinValue, kMaxValue);
      exec->Put(i, j);
    }
  });

  worker1.join();
  worker2.join();
  worker3.join();
}
}  // namespace coarse
