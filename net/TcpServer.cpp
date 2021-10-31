#include "TcpServer.h"
#include "../base/Logging.h"
#include "Sockets.h"

namespace mihooke {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr, int threadNum)
    : _loop(loop),
      _acceptor(new Acceptor(loop, addr)),
      _pool(new EventLoopThreadPool(loop, threadNum)) {
  _acceptor->setReadCallback(
      std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
  for (auto &conn : _connections) {
    TcpConnectionPtr connection(conn.second);
    conn.second.reset();
    connection->destory();
  }
}

void TcpServer::start() {
  _pool->start(_threadPoolInitCb);
  _acceptor->listen();
}

void TcpServer::newConnection(int fd, const InetAddress &clientAddr) {
  EventLoop *loop = _pool->getNextLoop();
  const InetAddress localAddr{getLocalAddr(fd)};
  TcpConnectionPtr conn(new TcpConnection(loop, fd, localAddr, clientAddr));
  _connections[fd] = conn;
  conn->setConnectionCallback(_connectionCb);
  conn->setMessageCallback(_messageCb);
  conn->setWriteCompleteCallback(_writeCompleteCb);
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  _connections.erase(conn->fd());
}

}  // namespace mihooke