#include "../net/InetAddress.h"
#include <assert.h>
#include "../base/Logging.h"

using namespace mihooke;

int main() {
  InetAddress addr1("192.168.80.207", 9527);
  assert(addr1.ip() == "192.168.80.207");
  assert(addr1.port() == 9527);
  assert(addr1.ipPort() == "192.168.80.207:9527");

  InetAddress addr2(9527);
  assert(addr2.ip() == "0.0.0.0");
  assert(addr2.port() == 9527);

  InetAddress addr3("192.168.80.207", 9527);
  assert(addr1 == addr3);

  return 0;
}