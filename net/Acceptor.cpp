#include "Acceptor.h"
#include <unistd.h>
#include "../base/Logging.h"
#include "Channel.h"
#include "INetAddress.h"
#include "Sockets.h"

namespace mihooke {
Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr)
    : _loop(loop),
      _sockets(new Sockets(createNonblockingSocket())),
      _channel(new Channel(loop, _sockets->fd())) {
  _sockets->setReuseAddr(true);
  _sockets->setReuseAddr(true);
  _sockets->bind(addr);
  _channel->setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
  _channel->disableAll();
  _channel->remove();
}

void Acceptor::listen() {
  _sockets->listen();
  _channel->enableReading();
}

void Acceptor::handleRead() {
  InetAddress clientAddr;
  int clientFd = _sockets->accept(clientAddr);
  if (clientFd >= 0) {
    if (_readCallback) {
      _readCallback(clientFd, clientAddr);
    } else {
      ::close(clientFd);
    }
  } else {
    LOG_ERROR << "Acceptor::handleRead clientFd<0 !";
  }
}
}  // namespace mihooke
