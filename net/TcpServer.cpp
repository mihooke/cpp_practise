#include "TcpServer.h"

namespace mihooke {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr)
    : _loop(loop), _acceptor(new Acceptor(loop, addr)) {
  _acceptor->setReadCallback(
      std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {}

void TcpServer::start(int num) {}
void TcpServer::stop() {}

void TcpServer::newConnection(int fd, const InetAddress &addr) {}

}  // namespace mihooke