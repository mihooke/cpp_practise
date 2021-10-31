#include "../net/TcpServer.h"
#include <memory>
#include "../base/Logging.h"
#include "../net/EventLoop.h"

using namespace mihooke;

void handleMessage(const TcpConnection::TcpConnectionPtr& conn, Buffer* buf) {
  LOG_INFO << "Receive message:" << buf->retrieveAllAsString() << "from"
           << conn->localAddr().port();
}

int main() {
  Logging::setLogLevel(LogLevel::INFO);
  EventLoop loop;
  InetAddress addr(9527);
  TcpServer server(&loop, addr);
  server.start();
  server.setMessageCallback(
      std::bind(&handleMessage, std::placeholders::_1, std::placeholders::_2));
  loop.loop();
  return 0;
}