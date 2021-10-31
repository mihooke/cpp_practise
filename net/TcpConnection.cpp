#include "TcpConnection.h"
#include "../base/Logging.h"
#include "Channel.h"
#include "Sockets.h"

#include <memory>

namespace mihooke {

TcpConnection::TcpConnection(EventLoop *loop, int fd,
                             const InetAddress &localAddr,
                             const InetAddress &peerAddr)
    : _loop(loop),
      _localAddr(localAddr),
      _peerAddr(peerAddr),
      _sockets(new Sockets(fd)),
      _channel(new Channel(loop, fd)),
      _state(DISCONNECTED) {
  _channel->setReadCallback(std::bind(&TcpConnection::handleRead, this));
  _channel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
  _channel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
}

TcpConnection::~TcpConnection() {}

int TcpConnection::fd() const { return _channel->fd(); }

void TcpConnection::send(const std::string &message) {
  send(message.data(), message.length());
}

void TcpConnection::send(const void *message, int len) {
  // 当前没有待写的数据，先尝试直接write，剩余的数据放到output
  // buffer中，下次可写时再发送
  auto written{0};
  auto remain{0};
  auto hasError{false};
  if (!_channel->hasWrite() && _outputBuffer.readableBytes() == 0) {
    written = write(fd(), message, len);
    if (written >= 0) {
      remain = len - written;
      if (remain == 0 && _writeCompleteCallback) {
        _writeCompleteCallback(shared_from_this());
      }
    } else {
      if (errno == EPIPE || errno == ECONNRESET) {
        hasError = true;
      }
    }
  }
  if (!hasError && remain > 0) {
    size_t oldLen = _outputBuffer.readableBytes();
    if (oldLen + remain > _highWaterMark && _highWaterMark) {
      _highWaterMarkCallback(shared_from_this(), oldLen + remain);
    }
    _outputBuffer.append(static_cast<const char *>(message) + written, remain);
    if (!_channel->hasWrite()) {
      _channel->enableWriting();
    }
  }
}

void TcpConnection::established() {
  _channel->enableReading();
  _state = CONNECTED;
  _connectionCallback(shared_from_this());
}

void TcpConnection::destory() {
  _channel->disableAll();
  _state = DISCONNECTED;
  _connectionCallback(shared_from_this());
  _channel->remove();
}

void TcpConnection::handleRead() {
  int errorNum{0};
  auto n = _inputBuffer.readFromFd(fd(), &errorNum);
  if (n > 0) {
    _messageCallback(shared_from_this(), &_inputBuffer);
  } else if (n == 0) {
    handleClose();
  } else {
    LOG_ERROR << "handleRead error" << n << errorNum;
  }
}

void TcpConnection::handleWrite() {
  if (!_channel->hasWrite()) {
    return;
  }
  auto n = write(fd(), _outputBuffer.peek(), _outputBuffer.readableBytes());
  if (n <= 0) {
    LOG_ERROR << "TcpConnection::handleWrite failed";
    return;
  }
  _outputBuffer.retrieve(n);
  if (_outputBuffer.readableBytes() == 0) {
    _channel->disableWriting();
    if (_writeCompleteCallback) {
      _writeCompleteCallback(shared_from_this());
    }
  }
}

void TcpConnection::handleClose() {
  _channel->disableAll();
  _state = DISCONNECTED;
  _connectionCallback(shared_from_this());
  _closeCallback(shared_from_this());
}

}  // namespace mihooke
