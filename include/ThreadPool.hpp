#ifndef RENDERENGINE_THREADPOOL_HPP
#define RENDERENGINE_THREADPOOL_HPP

#include <barrier>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace RenderEngine {
class ThreadPool {
  using TaskType = std::function<void()>;
  std::vector<std::thread> threads_;
  std::vector<TaskType> tasks_;
  std::mutex tasksMutex_;
  std::condition_variable tasksConditionVariable_;
  bool exitFlag_;
  bool runFlag_;
  size_t workerCount_;
  std::barrier<std::function<void()>> round_;

  ThreadPool();

public:
  static ThreadPool &getInstance();

  void addTask(TaskType task);

  void startAndWait();

  ~ThreadPool();
};
} // namespace RenderEngine

#endif // RENDERENGINE_THREADPOOL_HPP
