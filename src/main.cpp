#include <netinet/tcp.h>
#include <sys/socket.h>
#include <trantor/utils/Logger.h>

#include <drogon/drogon.h>
using namespace drogon;

int main() {
  app()
      .setBeforeListenSockOptCallback([](int fd) {
        LOG_INFO << "setBeforeListenSockOptCallback: " << fd;

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
      })
      .setAfterAcceptSockOptCallback([](int) {});

  LOG_INFO << "Server running on all interfaces: *:9007";
  app().addListener("::", 9007).run();
}
