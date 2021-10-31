#ifndef MIHOOKE_CHANNEL_H
#define MIHOOKE_CHANNEL_H

#include <poll.h>
#include <functional>

namespace mihooke {

class EventLoop;
class Channel {
 public:
  using EventCallback = std::function<void()>;
  enum Event {
    NONE = 0,
    READ = POLLIN | POLLPRI,
    WRITE = POLLOUT,
  };
  enum State {
    NEW = 0,
    ADDED = 1,
    DELETED = 2,
  };

 public:
  Channel(EventLoop* loop, int fd);
  ~Channel();

  void handleEvent();
  void setReadCallback(const EventCallback& cb) { _readCallback = cb; }
  void setWriteCallback(const EventCallback& cb) { _writeCallback = cb; }
  void setCloseCallback(const EventCallback& cb) { _closeCallback = cb; }
  void setErrorCallback(const EventCallback& cb) { _errorCallback = cb; }

  void setREvents(int e) { _revents = e; }
  void enableReading() {
    _events |= READ;
    update();
  }
  void disableReading() {
    _events &= ~READ;
    update();
  }
  void enableWriting() {
    _events |= WRITE;
    update();
  }
  void disableWriting() {
    _events &= ~WRITE;
    update();
  }
  void disableAll() { _events = NONE; }
  void remove();

  int fd() const { return _fd; }
  int events() const { return _events; }
  EventLoop* ownerLoop() { return _loop; }
  bool isNoneEvent() const { return _events == NONE; }
  bool hasWrite() const { return _events & WRITE; }
  bool hasRead() const { return _events & READ; }

  State state() const { return _state; }
  void setState(State state) { _state = state; }

 private:
  void update();

 private:
  EventLoop* _loop;
  const int _fd;
  EventCallback _readCallback;
  EventCallback _writeCallback;
  EventCallback _closeCallback;
  EventCallback _errorCallback;
  int _events;
  int _revents;
  State _state;
};

}  // namespace mihooke

#endif