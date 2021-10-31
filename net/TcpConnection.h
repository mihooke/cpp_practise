#ifndef MIHOOKE_TCPCONNECTION_H
#define MIHOOKE_TCPCONNECTION_H

#include <any>
#include <functional>
#include <memory>
#include <string>

#include "Buffer.h"
#include "INetAddress.h"

namespace mihooke {

class Channel;
class EventLoop;
class Sockets;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
  using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
  using HighWaterMarkCallback =
      std::function<void(const TcpConnectionPtr &, size_t)>;
  using MessageCallback =
      std::function<void(const TcpConnectionPtr &, Buffer *)>;

  enum State {
    CONNECTED,
    DISCONNECTED,
  };

 public:
  TcpConnection(EventLoop *loop, int fd, const InetAddress &localAddr,
                const InetAddress &peerAddr);
  ~TcpConnection();

  void setConnectionCallback(const ConnectionCallback &cb) {
    _connectionCallback = cb;
  }
  void setMessageCallback(const MessageCallback &cb) { _messageCallback = cb; }
  void setWriteCompleteCallback(const WriteCompleteCallback &cb) {
    _writeCompleteCallback = cb;
  }
  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb) {
    _highWaterMarkCallback = cb;
  }
  void setCloseCallback(const CloseCallback &cb) { _closeCallback = cb; }

  int fd() const;
  const InetAddress &localAddr() const { return _localAddr; }
  const InetAddress &peerAddr() const { return _peerAddr; }
  bool isConnected() const { return _state == CONNECTED; }

  void send(const std::string &message);
  void send(const void *message, int len);

  void established();
  void destory();

 private:
  void handleRead();
  void handleWrite();
  void handleClose();

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
  State _state;
  ConnectionCallback _connectionCallback;
  MessageCallback _messageCallback;
  WriteCompleteCallback _writeCompleteCallback;
  HighWaterMarkCallback _highWaterMarkCallback;
  CloseCallback _closeCallback;
};

}  // namespace mihooke

#endif