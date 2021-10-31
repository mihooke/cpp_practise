#include "EventLoopThread.h"
#include "EventLoop.h"

namespace mihooke {
EventLoopThread::EventLoopThread(const std::string& name)
    : _thread(std::bind(&EventLoopThread::run, this), name),
      _loop(nullptr),
      _name(name),
      _cond(_mutex) {}

EventLoopThread::~EventLoopThread() {
  if (_loop != nullptr) {
    _loop->quit();
    _thread.join();
  }
}

EventLoop* EventLoopThread::start() {
  _thread.start();
  EventLoop* loop = nullptr;
  {
    MutexLockGuard lg(_mutex);
    while (_loop == nullptr) {
      _cond.wait();
    }
    loop = _loop;
  }
  return loop;
}
void EventLoopThread::run() {
  EventLoop loop;
  {
    MutexLockGuard lg(_mutex);
    _loop = &loop;
    _cond.notify();
  }
  loop.loop();
}
}  // namespace mihooke
