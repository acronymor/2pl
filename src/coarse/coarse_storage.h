#include <iostream>
#include <mutex>
#include <vector>

namespace coarse {

#define LOCATE(i, len) ((i) % len)

class Storage {
 public:
  Storage(const std::vector<uint32_t> vectors) : vector_(vectors) {
    size_ = vector_.size();
  }

  void Put(int i, int j) {
    std::unique_lock<std::mutex> lock_guard(mutex_);

    int a = vector_[LOCATE(i, size_)];
    int b = vector_[LOCATE(i + 1, size_)];
    int c = vector_[LOCATE(i + 2, size_)];
    int d = a + b + c;

    vector_[LOCATE(j, size_)] = d;

    std::cout << i << "-" << a << "," << i + 1 << "-" << b << "," << i + 2 << "-" << c << ",d-" << d;
    std::cout << "," << j << "-" << vector_[LOCATE(j, size_)] << std::endl;
  }

 private:
  std::vector<uint32_t> vector_;
  std::size_t size_;
  std::mutex mutex_;
};
}// namespace coarse
