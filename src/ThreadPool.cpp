#include "ThreadPool.hpp"


#ifdef DEBUG
#ifndef RENDERENGINE_THREADPOOL_WORKERCOUNT
#define RENDERENGINE_THREADPOOL_WORKERCOUNT 1
#endif
#else
#ifndef RENDERENGINE_THREADPOOL_WORKERCOUNT
#define RENDERENGINE_THREADPOOL_WORKERCOUNT (std::thread::hardware_concurrency())
#endif
#endif


namespace RenderEngine {
using TaskType = std::function<void()>;

ThreadPool::ThreadPool()
    : exitFlag_(false), runFlag_(false),
      workerCount_(RENDERENGINE_THREADPOOL_WORKERCOUNT),
      round_(static_cast<ptrdiff_t>(workerCount_ + 1), [this]() {
        std::unique_lock lock(tasksMutex_);
        runFlag_ = false;
      }) {
  threads_.reserve(workerCount_);
  for (size_t i = 0; i < workerCount_; ++i) {
    threads_.emplace_back([this]() {
      while (true) {
        {
          std::unique_lock lock(tasksMutex_);
          tasksConditionVariable_.wait(
              lock, [this]() { return runFlag_ || exitFlag_; });
          if (exitFlag_ && tasks_.empty()) {
            return;
          }
        }
        while (true) {
          TaskType task;
          {
            std::unique_lock lock(tasksMutex_);
            if (tasks_.empty()) {
              break;
            }
            task = std::move(tasks_.back());
            tasks_.pop_back();
          }
          task();
        }
        round_.arrive_and_wait();
      };
    });
  }
}

ThreadPool &ThreadPool::getInstance() {
  static ThreadPool singleton_;
  return singleton_;
}

void ThreadPool::addTask(TaskType task) {
  std::unique_lock lock(tasksMutex_);
  if (runFlag_) {
    throw std::runtime_error(
        "Tried to add buckets while rendering. This should not happen!");
  }
  tasks_.push_back(task);
}

void ThreadPool::startAndWait() {
  {
    std::unique_lock lock(tasksMutex_);
    if (runFlag_) {
      throw std::runtime_error("Tried to start rendering while already under "
                               "way. This should not happen!");
    }
    runFlag_ = true;
  }
  tasksConditionVariable_.notify_all();
  round_.arrive_and_wait();
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock lock(tasksMutex_);
    exitFlag_ = true;
    runFlag_ = true;
  }
  tasksConditionVariable_.notify_all();

  for (auto &thread : threads_) {
    thread.join();
  }
}
} // namespace RenderEngine