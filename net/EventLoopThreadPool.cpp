#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

namespace mihooke {

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int num)
    : _baseLoop(baseLoop), _threadNum(num), _currentIndex(0) {}

EventLoopThreadPool::~EventLoopThreadPool() {}

void EventLoopThreadPool::start(const ThreadPoolInitCallback& cb) {
  for (int i = 0; i < _threadNum; i++) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%d", "EventLoopThread_", i);
    EventLoopThread* thread = new EventLoopThread(buf);
    _threads.emplace_back(thread);
    _loops.push_back(thread->start());
  }
  if (cb) {
    cb(_baseLoop);
  }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
  EventLoop* loop = _baseLoop;
  if (!_loops.empty()) {
    loop = _loops[_currentIndex];
    ++_currentIndex;
    if (_currentIndex >= _loops.size()) {
      _currentIndex = 0;
    }
  }
  return loop;
}

}  // namespace mihooke
