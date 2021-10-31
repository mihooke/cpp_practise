#ifndef MIHOOKE_TCPSERVER_H
#define MIHOOKE_TCPSERVER_H

#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"

#include <map>
#include <memory>
#include <vector>

namespace mihooke {

using std::placeholders::_1;
using std::placeholders::_2;
using TcpConnectionPtr = TcpConnection::TcpConnectionPtr;

class EventLoop;
class TcpServer {
 public:
  TcpServer(EventLoop *loop, const InetAddress &addr, int threadNum = 0);
  ~TcpServer();

  void start();

  void setConnectionCallback(const TcpConnection::ConnectionCallback &cb) {
    _connectionCb = cb;
  }
  void setMessageCallback(const TcpConnection::MessageCallback &cb) {
    _messageCb = cb;
  }
  void setWriteCompleteCallback(const TcpConnection::WriteCompleteCallback cb) {
    _writeCompleteCb = cb;
  }
  void setThreadPoolInitCallback(
      const EventLoopThreadPool::ThreadPoolInitCallback &cb) {
    _threadPoolInitCb = cb;
  }

  void newConnection(int fd, const InetAddress &clientAddr);
  void removeConnection(const TcpConnectionPtr &conn);

 private:
  EventLoop *_loop;
  std::unique_ptr<Acceptor> _acceptor;
  std::unique_ptr<EventLoopThreadPool> _pool;
  std::map<int, TcpConnectionPtr> _connections;
  TcpConnection::ConnectionCallback _connectionCb;
  TcpConnection::MessageCallback _messageCb;
  TcpConnection::WriteCompleteCallback _writeCompleteCb;
  EventLoopThreadPool::ThreadPoolInitCallback _threadPoolInitCb;
};

}  // namespace mihooke

#endif