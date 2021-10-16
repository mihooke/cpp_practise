#include "Channel.h"
#include "EventLoop.h"

namespace mihooke {

Channel::Channel(EventLoop *loop, int fd)
    : _loop(loop), _fd(fd), _events(NONE), _revents(0), _state(NEW) {}

Channel::~Channel() {}

void Channel::handleEvent() {
  if ((_revents & POLLHUP) && !(_revents & POLLIN)) {
    if (_closeCallback) {
      _closeCallback();
    }
  }
  if (_revents & (POLLERR | POLLNVAL)) {
    if (_errorCallback) {
      _errorCallback();
    }
  }
  if (_revents & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (_readCallback) {
      _readCallback();
    }
  }
  if (_revents & POLLOUT) {
    if (_writeCallback) {
      _writeCallback();
    }
  }
}

void Channel::remove() { _loop->removeChannel(this); }

void Channel::update() { _loop->updateChannel(this); }

}  // namespace mihooke