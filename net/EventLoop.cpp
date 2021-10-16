#include "EventLoop.h"
#include "../base/Logging.h"
#include "Channel.h"
#include "Epoller.h"
#include "Sockets.h"

#include <sys/eventfd.h>
#include <unistd.h>

namespace {
__thread mihooke::EventLoop *t_threadLoop = nullptr;

int createEventFd() {
  int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (fd < 0) {
    LOG_FATAL << "eventfd failed!";
  }
  return fd;
}

}  // namespace

namespace mihooke {

EventLoop::EventLoop()
    : _quit(false),
      _poller(new Epoller(this)),
      _wakeupFd(createEventFd()),
      _wakeupChannel(new Channel(this, _wakeupFd)),
      _isHandling(false),
      _isDoingTask(false),
      _tid(gettid()) {
  if (t_threadLoop) {
    LOG_FATAL << "EventLoop already exists in this thread";
  }
  t_threadLoop = this;
  _wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
  _wakeupChannel->enableReading();  // update wakeupFd到poller中
}

EventLoop::~EventLoop() {
  _wakeupChannel->diableAll();
  _wakeupChannel->remove();
  ::close(_wakeupFd);
  t_threadLoop = nullptr;
}

void EventLoop::loop() {
  while (!_quit) {
    std::vector<Channel *> activeChannels = _poller->poll(1000);
    _isHandling = true;
    for (const auto &channel : activeChannels) {
      channel->handleEvent();
    }
    _isHandling = false;
    doLoopTasks();
  }
}

void EventLoop::quit() {
  _quit = true;
  if (!isInLoopThread()) {
    wakeup();
  }
}

void EventLoop::runInLoop(Functor cb) {
  {
    MutexLockGuard lg(_mutex);
    _functors.push_back(std::move(cb));
  }
  if (!isInLoopThread() || _isDoingTask) {
    wakeup();
  }
}

void EventLoop::updateChannel(Channel *channel) {
  _poller->updateChannel(channel);
}
void EventLoop::removeChannel(Channel *channel) {
  _poller->removeChannel(channel);
}

void EventLoop::doLoopTasks() {
  _isDoingTask = true;
  std::vector<Functor> fs;
  {
    MutexLockGuard lg(_mutex);
    _functors.swap(fs);
  }
  for (const auto &f : fs) {
    f();
  }
  _isDoingTask = false;
}
void EventLoop::wakeup() {
  const int64_t num{9527};
  ssize_t n = write(_wakeupFd, &num, sizeof(int64_t));
  if (n != sizeof(int64_t)) {
    LOG_ERROR << "write wakeupFd failed!";
  }
}
void EventLoop::handleRead() {
  int64_t v{0};
  ssize_t n = read(_wakeupFd, &v, sizeof(int64_t));
  if (n != sizeof(int64_t)) {
    LOG_ERROR << "read wakeupFd failed!";
  }
}
}  // namespace mihooke