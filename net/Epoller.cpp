#include "Epoller.h"
#include "../base/Logging.h"
#include "Channel.h"

#include <errno.h>
#include <unistd.h>

namespace mihooke {

Epoller::Epoller(EventLoop *loop)
    : _loop(loop), _epollfd(::epoll_create1(EPOLL_CLOEXEC)), _events(16) {
  if (_epollfd < 0) {
    LOG_FATAL << "Epoller epoll_create1 failed!";
  }
}

Epoller::~Epoller() { ::close(_epollfd); }

// int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int
// timeout);
// EINTR  The call was interrupted by a signal handler before either
// (1) any of the requested events occurred or (2) the timeout expired; see
// signal(7).
std::vector<Channel *> Epoller::poll(int timeoutMs) {
  int numOfEvents =
      ::epoll_wait(_epollfd, _events.data(), _events.size(), timeoutMs);
  int originErrorno = errno;
  if (numOfEvents < 0) {
    if (originErrorno != EINTR) {
      errno = originErrorno;
      LOG_ERROR << "Epoller epoll_wait Error!";
    }
    return {};
  }
  if (numOfEvents > 0) {
    std::vector<Channel *> channels;
    for (int i = 0; i < numOfEvents; i++) {
      Channel *channel = static_cast<Channel *>(_events[i].data.ptr);
      channel->setREvents(_events[i].events);
      channels.push_back(channel);
    }
    if (numOfEvents == _events.size()) {
      _events.resize(_events.size() * 2);
    }
    return channels;
  }
  return {};
}
void Epoller::updateChannel(Channel *channel) {
  const Channel::State state = channel->state();
  if (state == Channel::State::NEW || state == Channel::State::DELETED) {
    _channels[channel->fd()] = channel;
    channel->setState(Channel::State::ADDED);
    controlEpoll(EPOLL_CTL_ADD, channel);
  } else {
    if (channel->isNoneEvent()) {
      controlEpoll(EPOLL_CTL_DEL, channel);
      channel->setState(Channel::State::DELETED);
    } else {
      controlEpoll(EPOLL_CTL_MOD, channel);
    }
  }
}
void Epoller::removeChannel(Channel *channel) {
  _channels.erase(channel->fd());
  if (channel->state() == Channel::State::ADDED) {
    controlEpoll(EPOLL_CTL_DEL, channel);
  }
  channel->setState(Channel::State::NEW);
}

void Epoller::controlEpoll(int operation, Channel *channel) {
  // event参数保存了所要关注fd的事件类型；ptr参数可保存额外数据，这里保存channel指针，可在有事件返回时直接拿到channal对象
  struct epoll_event event;
  memset(&event, 0, sizeof(event));
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  if (::epoll_ctl(_epollfd, operation, fd, &event) < 0) {
    LOG_ERROR << "epoll_ctl" << operation << "failed!";
  }
}
}  // namespace mihooke
