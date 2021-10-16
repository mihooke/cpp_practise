#ifndef MIHOOKE_EVENTLOOP_H
#define MIHOOKE_EVENTLOOP_H

#include <any>
#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include "../base/CurrentThread.h"
#include "../base/Mutex.h"

namespace mihooke {

class Channel;
class Epoller;
class EventLoop {
 public:
  using Functor = std::function<void()>;

 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void quit();
  void runInLoop(Functor cb);

  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);

 private:
  void wakeup();
  void handleRead();
  void doLoopTasks();
  bool isInLoopThread() const { return _tid == gettid(); }

 private:
  std::atomic<bool> _quit;
  std::unique_ptr<Epoller> _poller;
  int _wakeupFd;
  std::unique_ptr<Channel> _wakeupChannel;
  std::vector<Channel *> _channels;
  bool _isHandling;
  bool _isDoingTask;
  std::any _context;
  Mutex _mutex;
  std::vector<Functor> _functors;
  const pid_t _tid;
};

}  // namespace mihooke

#endif