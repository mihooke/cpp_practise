#ifndef MIHOOKE_TCPSERVER_H
#define MIHOOKE_TCPSERVER_H

#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"

#include <map>
#include <memory>
#include <vector>

namespace mihooke {

using std::placeholders::_1;
using std::placeholders::_2;

class EventLoop;
class TcpServer {
 public:
  TcpServer(EventLoop *loop, const InetAddress &addr);
  ~TcpServer();

  void start(int num);
  void stop();

  void newConnection(int fd, const InetAddress &addr);

 private:
  EventLoop *_loop;
  std::unique_ptr<Acceptor> _acceptor;
  std::vector<EventLoop> _loops;
  std::map<std::string, TcpConnection::TcpConnectionPtr> _connections;
  TcpConnection::ConnectionCallback _connectionCb;
  TcpConnection::MessageCallback _messageCb;
  TcpConnection::WriteCompleteCallback _writeCompleteCb;
};

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr)
    : _loop(loop), _acceptor(new Acceptor(loop, addr)) {
  _acceptor->setReadCallback(
      std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {}

}  // namespace mihooke

#endif