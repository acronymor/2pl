# Question

给定一个长度为 N 的数组 S，然后由多个 worker 并发访问并更新 S。每个 worker 重复 M 次如下操作。

1. 随机生成 i, j (i，j <= i < N)
2. 更新 N 使得 S[j] = s[i] + s[i+1] + s[i+2]

> 如果 i+1 或 i+2 越界，则 `(i+1) % N` 或者 `(i+2) % N`

# Answer

## src/coarse

采用悲观策略 + 粗粒度锁，认为对数组 S 的任何操作都应该加互斥锁处理，这样无论是读还是写都保证线程安全。

## src/fine

采用乐观策略 + 细粒度锁，认为对数组 S 的写操作应该加互斥锁，对于读操作加共享锁。将锁粒度细化到下标，而非整个数组。

# Reference

- [15-445-Lec16-两阶段锁](https://zhuanlan.zhihu.com/p/480379228)
- [cmu_15445_2018](https://github.com/gatsbyd/cmu_15445_2018)