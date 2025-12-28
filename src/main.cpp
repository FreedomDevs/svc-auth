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
        LOG_INFO << "setBeforeListenSockOptCallback:" << fd;
        int enable = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_FASTOPEN, &enable,
                       sizeof(enable)) == -1) {
          LOG_INFO << "setsockopt TCP_FASTOPEN failed";
        }
      })
      .setAfterAcceptSockOptCallback([](int) {});

  LOG_INFO << "Server running on 127.0.0.1:9005";
  app().addListener("127.0.0.1", 9007).run();
}
