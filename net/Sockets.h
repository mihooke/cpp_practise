#ifndef MIHOOKE_SOCKETS_H
#define MIHOOKE_SOCKETS_H

#include <arpa/inet.h>
#include <string>

namespace mihooke {

class InetAddress;
class Sockets {
 public:
  explicit Sockets(int fd) : _fd(fd) {}
  ~Sockets();
  Sockets(Sockets&& rhs) : Sockets(rhs._fd) { rhs._fd = -1; }

  int fd() const { return _fd; }

  void bind(const InetAddress& addr);
  void listen();
  int accept(InetAddress& clientAddr);

  void shutdownWrite();

  void setReuseAddr(bool on);
  void setReusePort(bool on);
  void setTcpNoDelay(bool on);
  void setKeepAlive(bool on);

  std::string getTcpInfo() const;

 private:
  int _fd;
};

int createNonblockingSocket();
int connect(int fd, const struct sockaddr* addr);
ssize_t read(int fd, void* buf, size_t len);
ssize_t readv(int fd, const struct iovec* iv, int len);
ssize_t write(int fd, const void* buf, size_t len);
void close(int fd);

struct sockaddr_in getLocalAddr(int fd);
struct sockaddr_in getPeerAddr(int fd);
bool isSelfConnect(int fd);

int getSocketError(int fd);

}  // namespace mihooke

#endif