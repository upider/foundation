# foundation

## 用途

一些c++基础库和工具库, 仅适用于linux平台, c++11(gcc (GCC) 8.3.1 20190311 (Red Hat 8.3.1-3)通过测试)

## utility

- [x] strings
- [x] date
- [x] defer
- [x] env
- [x] singleton
- [x] numerics

## stack_trace

需要[backward-cpp](https://github.com/bombela/backward-cpp)和[elfutils](https://fedorahosted.org/elfutils/)

- [x] stacktrace_str
- [x] stacktrace_vec

## concurrency

- [x] thread_local_ptr
- [x] thread_factory
- [x] thread utilities (thread.hpp)
- [x] semaphore
- [x] signal
- [x] simple_timer
- [x] concurrent_queues
  - [x] array_blocking_queue
  - [x] delay_queue
  - [x] linked_blocking_queue
  - [x] priority_blocking_queue
- [x] executor
  - [x] thread_pool_executor
  - [x] priority_executor
  - [x] schedule_executor
- [x] mutex
  - [x]  null_mutex
  - [x]  rw_mutex
  - [x]  spin_mutex

## TODO

- [ ] HeapTimer
- [ ] WheelTimer
- [ ] HierarchicalWheelTimer
- [ ] concurrent hash map
- [ ] rbtree、btree、b+tree、skiplist、lru
- [ ] tcp、udp、http
- [ ] Selector + Channel
