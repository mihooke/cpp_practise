#include "Sockets.h"
#include "../base/Logging.h"
#include "INetAddress.h"

#include <netinet/in.h>   //sockaddr_in
#include <netinet/tcp.h>  //tcp_info
#include <sys/socket.h>   //bind listen shutdown setsockopt accept sockaddr
#include <sys/uio.h>      //readv
#include <unistd.h>

namespace mihooke {

Sockets::~Sockets() { close(_fd); }

void Sockets::bind(const InetAddress& addr) {
  int ret = ::bind(_fd, addr.getSockAddr(), addr.length());
  if (ret < 0) {
    LOG_FATAL << "Sockets::bindOrDie failed";
  }
}

void Sockets::listen() {
  int ret = ::listen(_fd, SOMAXCONN);
  if (ret < 0) {
    LOG_FATAL << "Sockets::listenOrDie failed";
  }
}
int Sockets::accept(InetAddress& clientAddr) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = static_cast<socklen_t>(sizeof(addr));
#ifdef NO_ACCEPT4
  int connfd =
      ::accept(_fd, static_cast<sockaddr*>(static_cast<void*>(&addr)), &len);
#else
  int connfd = ::accept4(_fd, static_cast<sockaddr*>(static_cast<void*>(&addr)),
                         &len, SOCK_CLOEXEC | SOCK_NONBLOCK);
#endif
  if (connfd < 0) {
    int err = errno;
    switch (err) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO:
      case EPERM:
      case EMFILE:
        errno = err;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        LOG_FATAL << "Unexpected error in accept" << err;
        break;
      default:
        LOG_FATAL << "Unknown error in accept" << err;
        break;
    }
  }
  clientAddr.setSockAddr(addr);
  return connfd;
}

void Sockets::shutdownWrite() {
  int ret = ::shutdown(_fd, SHUT_WR);
  if (ret < 0) {
    LOG_FATAL << "Sockets::shutdownWrite failed";
  }
}

void Sockets::setReuseAddr(bool on) {
  int val = on ? 1 : 0;
  int ret = ::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
  if (ret < 0) {
    LOG_ERROR << "Sockets::setReuseAddr failed";
  }
}
void Sockets::setTcpNoDelay(bool on) {
  int val = on ? 1 : 0;
  int ret = ::setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
  if (ret < 0) {
    LOG_ERROR << "Sockets::setTcpNoDelay failed";
  }
}

void Sockets::setReusePort(bool on) {
#ifdef SO_REUSEPORT
  int val = on ? 1 : 0;
  int ret = ::setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
  if (ret < 0 && on) {
    LOG_FATAL << "Sockets::setReusePort failed";
  }
#else
  if (on) {
    LOG_ERROR << "SO_REUSEPORT not supported!"
  }
#endif
}

void Sockets::setKeepAlive(bool on) {
  int val = on ? 1 : 0;
  int ret = ::setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
  if (ret < 0) {
    LOG_ERROR << "Sockets::setKeepAlive failed";
  }
}

std::string Sockets::getTcpInfo() const {
  struct tcp_info ti;
  socklen_t len = sizeof(ti);
  int ret = ::getsockopt(_fd, SOL_TCP, TCP_INFO, &ti, &len);
  if (ret == 0) {
    char buf[128];
    snprintf(
        buf, sizeof(buf),
        "unrecovered=%u, rto=%u, ato=%u, snd_mss=%u, rcv_mss=%u, lost=%u, "
        "retans=%u, rtt=%u, rttvar=%u, sshthresh=%u, cwnd=%u, total_retrans=%u",
        ti.tcpi_retransmits, ti.tcpi_rto, ti.tcpi_ato, ti.tcpi_snd_mss,
        ti.tcpi_rcv_mss, ti.tcpi_lost, ti.tcpi_retrans, ti.tcpi_rtt,
        ti.tcpi_rttvar, ti.tcpi_snd_ssthresh, ti.tcpi_snd_cwnd,
        ti.tcpi_total_retrans);
    return buf;
  }
  return {};
}

int createNonblockingSocket() {
  int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    IPPROTO_TCP);
  if (fd < 0) {
    LOG_FATAL << "createNonblockingSocket failed!";
  }
  return fd;
}

int connect(int fd, const struct sockaddr* addr) {
  return ::connect(fd, addr, static_cast<socklen_t>(sizeof(*addr)));
}

ssize_t read(int fd, void* buf, size_t len) { return ::read(fd, buf, len); }
ssize_t readv(int fd, const struct iovec* iv, int len) {
  return ::readv(fd, iv, len);
}

ssize_t write(int fd, const void* buf, size_t len) {
  return ::write(fd, buf, len);
}

void close(int fd) {
  if (::close(fd) < 0) {
    LOG_FATAL << "close failed!";
  }
}

struct sockaddr_in getLocalAddr(int fd) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = static_cast<socklen_t>(sizeof(addr));
  if (::getsockname(fd, static_cast<sockaddr*>(static_cast<void*>(&addr)),
                    &len) < 0) {
    LOG_ERROR << "getLocalAddr failed!";
  }
  return addr;
}

struct sockaddr_in getPeerAddr(int fd) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  socklen_t len = static_cast<socklen_t>(sizeof(addr));
  if (::getpeername(fd, static_cast<sockaddr*>(static_cast<void*>(&addr)),
                    &len) < 0) {
    LOG_ERROR << "getPeerAddr failed!";
  }
  return addr;
}

bool isSelfConnect(int fd) {
  const struct sockaddr_in local = getLocalAddr(fd);
  const struct sockaddr_in peer = getPeerAddr(fd);
  return local.sin_port == peer.sin_port &&
         local.sin_addr.s_addr == peer.sin_addr.s_addr;
}

int getSocketError(int fd) {
  int val{0};
  socklen_t len = static_cast<socklen_t>(sizeof(val));
  if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &val, &len) < 0) {
    return errno;
  }
  return val;
}

}  // namespace mihooke
