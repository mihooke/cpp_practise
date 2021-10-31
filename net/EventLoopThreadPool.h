#ifndef MIHOOKE_EVENTLOOPTHREADPOOL_H
#define MIHOOKE_EVENTLOOPTHREADPOOL_H

#include <functional>
#include <memory>
#include <vector>

namespace mihooke {

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool {
 public:
  using ThreadPoolInitCallback = std::function<void(EventLoop*)>;

 public:
  EventLoopThreadPool(EventLoop* baseLoop, int num = 0);
  ~EventLoopThreadPool();

  void start(const ThreadPoolInitCallback& cb);
  EventLoop* getNextLoop();

 private:
  EventLoop* _baseLoop;
  int _threadNum;
  size_t _currentIndex;
  std::vector<std::unique_ptr<EventLoopThread>> _threads;
  std::vector<EventLoop*> _loops;
};

}  // namespace mihooke

#endif