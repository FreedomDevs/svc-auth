#include <netinet/tcp.h>
#include <sys/socket.h>
#include <trantor/utils/Logger.h>

#include <drogon/drogon.h>
using namespace drogon;

void configurateSocket(int fd) {
  // Включаем TCP_FASTOPEN
  int enable = 1;
  if (setsockopt(fd, IPPROTO_TCP, TCP_FASTOPEN, &enable, sizeof(enable)) == -1) {
    LOG_INFO << "setsockopt TCP_FASTOPEN failed";
  }

  // Вклчаем dual-stack, IPv6 + IPv4
  int off = 0;
  if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off)) == -1) {
    LOG_INFO << "setsockopt IPV6_V6ONLY failed";
  }
}

int main() {
  app().setBeforeListenSockOptCallback(configurateSocket).setAfterAcceptSockOptCallback([](int) {}).addListener("::", 9007);

  LOG_INFO << "Server running on: *:9007";
  app().run();
}
