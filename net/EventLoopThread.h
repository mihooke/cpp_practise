#ifndef MIHOOKE_EVENTLOOPTHREAD_H
#define MIHOOKE_EVENTLOOPTHREAD_H

#include <string>

#include "../base/Condition.h"
#include "../base/Thread.h"

namespace mihooke {

class EventLoop;
class EventLoopThread {
 public:
  EventLoopThread(const std::string& name);
  ~EventLoopThread();
  EventLoop* start();
  void run();

 private:
  Thread _thread;
  EventLoop* _loop;
  std::string _name;
  Mutex _mutex;
  Condition _cond;
};

}  // namespace mihooke

#endif