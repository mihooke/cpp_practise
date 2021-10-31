#ifndef MIHOOKE_INETADDRESS_H
#define MIHOOKE_INETADDRESS_H

#include <netinet/in.h>  // sockaddr_in
#include <sys/socket.h>  // sockaddr
#include <string>

namespace mihooke {

class InetAddress {
 public:
  InetAddress() = default;
  InetAddress(const std::string &ip, uint16_t port);
  explicit InetAddress(uint16_t port);
  InetAddress(const struct sockaddr_in &saddr);

  void setSockAddr(const struct sockaddr_in &addr) { _addr = addr; }
  const struct sockaddr *getSockAddr() const {
    return reinterpret_cast<const sockaddr *>(&_addr);
  }

  bool operator==(const InetAddress &other) const;

  int length() const { return sizeof(_addr); }
  sa_family_t family() const { return _addr.sin_family; }
  std::string ip() const;
  std::string ipPort() const;
  uint16_t port() const;

  uint32_t ipNetEndian() const { return _addr.sin_addr.s_addr; }
  uint16_t portNetEndian() const { return _addr.sin_port; }

 private:
  struct sockaddr_in _addr;
};

}  // namespace mihooke

#endif