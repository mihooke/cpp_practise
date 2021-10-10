#include "INetAddress.h"
#include <arpa/inet.h>  // inet_pton inet_ntop
#include <assert.h>
#include <string.h>

namespace mihooke {

InetAddress::InetAddress(const std::string &ip, uint16_t port) {
  _addr.sin_family = AF_INET;
  _addr.sin_port = htons(port);
  int result = inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr.s_addr);
  assert(result == 1);
}

InetAddress::InetAddress(uint16_t port) {
  memset(&_addr, 0, sizeof(_addr));
  _addr.sin_family = AF_INET;
  _addr.sin_port = htons(port);
  _addr.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const struct sockaddr_in &addr) : _addr(addr) {}

bool InetAddress::operator==(const InetAddress &other) const {
  if (family() == other.family() && family() == AF_INET) {
    return _addr.sin_port == other._addr.sin_port &&
           _addr.sin_addr.s_addr == other._addr.sin_addr.s_addr;
  }
  return false;
}

std::string InetAddress::ip() const {
  char buf[64];
  // const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
  inet_ntop(AF_INET, &_addr.sin_addr, buf, sizeof(buf));
  return buf;
}

std::string InetAddress::ipPort() const {
  char buf[16];
  snprintf(buf, sizeof(buf), ":%u", port());
  return ip() + buf;
}

uint16_t InetAddress::port() const { return ntohs(_addr.sin_port); }

}  // namespace mihooke