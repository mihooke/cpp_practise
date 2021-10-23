#ifndef MIHOOKE_ACCEPTOR_H
#define MIHOOKE_ACCEPTOR_H

#include <functional>
#include <memory>

namespace mihooke {
class InetAddress;
class Channel;
class Sockets;
class EventLoop;
class Acceptor {
 public:
  using ReadCallback = std::function<void(int fd, const InetAddress& addr)>;

 public:
  Acceptor(EventLoop* loop, const InetAddress& addr);
  ~Acceptor();

  void setReadCallback(ReadCallback cb) { _readCallback = std::move(cb); }
  void listen();

  void handleRead();

 private:
  EventLoop* _loop;
  std::unique_ptr<Sockets> _sockets;
  std::unique_ptr<Channel> _channel;
  ReadCallback _readCallback;
};

}  // namespace mihooke

#endif