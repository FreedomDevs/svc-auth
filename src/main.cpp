#include "test.hpp"

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <trantor/utils/Logger.h>

#include <drogon/drogon.h>
using namespace drogon;

int main() {
  LOG_INFO << helloWorld();

  app().registerHandler(
      "/",
      [](const HttpRequestPtr &request,
         std::function<void(const HttpResponsePtr &)> &&callback) {
        LOG_INFO << "connected:" << (request->connected() ? "true" : "false");
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody("Hello, World!");
        callback(resp);
      },
      {Get});

  app()
      .setBeforeListenSockOptCallback([](int fd) {
        LOG_INFO << "setBeforeListenSockOptCallback: " << fd;

        // Включаем TCP_FASTOPEN
        int enable = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_FASTOPEN, &enable,
                       sizeof(enable)) == -1) {
          LOG_INFO << "setsockopt TCP_FASTOPEN failed";
        }

        // Для IPv6 сокетов разрешаем dual-stack
        int off = 0;
        if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off)) ==
            -1) {
          LOG_INFO << "setsockopt IPV6_V6ONLY failed";
        }
      })
      .setAfterAcceptSockOptCallback([](int) {});

  LOG_INFO << "Server running on all interfaces: 0.0.0.0 and :: on port 9007";

  app().addListener("::", 9007);

  app().run();
}
