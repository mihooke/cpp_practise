#ifndef MIHOOKE_EPOLLER_H
#define MIHOOKE_EPOLLER_H

#include <sys/epoll.h>
#include <map>
#include <vector>
#include "EventLoop.h"

namespace mihooke {

class Channel;
class Epoller {
 public:
  Epoller(EventLoop *loop);
  ~Epoller();

  std::vector<Channel *> poll(int timeoutMs);
  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);

 private:
  void controlEpoll(int operation, Channel *channel);

 private:
  EventLoop *_loop;
  int _epollfd;
  std::vector<epoll_event> _events;
  std::map<int, Channel *> _channels;
};

}  // namespace mihooke

#endif