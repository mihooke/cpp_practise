#ifndef MIHOOKE_TCPCONNECTION_H
#define MIHOOKE_TCPCONNECTION_H

#include <any>
#include <functional>
#include <memory>

#include "Buffer.h"
#include "INetAddress.h"

namespace mihooke {

class Channel;
class EventLoop;
class Sockets;
class TcpConnection {
 public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
  using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
  using HighWaterMarkCallback =
      std::function<void(const TcpConnectionPtr &, size_t)>;
  using MessageCallback =
      std::function<void(const TcpConnectionPtr &, Buffer *)>;

 public:
  TcpConnection(EventLoop *loop, int fd, const InetAddress &localAddr,
                const InetAddress &peerAddr);
  ~TcpConnection();

 private:
  EventLoop *_loop;
  Buffer _inputBuffer;
  Buffer _outputBuffer;
  InetAddress _localAddr;
  InetAddress _peerAddr;
  std::unique_ptr<Sockets> _sockets;
  std::unique_ptr<Channel> _channel;
  std::any _context;
  size_t _highWaterMark;
  ConnectionCallback _connectionCallback;
  MessageCallback _messageCallback;
  WriteCompleteCallback _writeCompleteCallback;
  HighWaterMarkCallback _highWaterMarkCallback;
  CloseCallback _closeCallback;
};

}  // namespace mihooke

#endif