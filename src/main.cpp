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
  auto client = drogon::orm::DbClient::newPgClient("dbname=svc-auth user=postgres password=postgres host=127.0.0.1 port=8007",
                                                   1 // кол-во соединений в пуле
  );
  app().registerHandler("/test", [client](const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    client->execSqlAsync(
        "SELECT 1",
        [callback](const orm::Result &r) {
          auto resp = HttpResponse::newHttpResponse();
          resp->setBody("DB OK, result = " + std::to_string(r[0][0].as<int>()));
          callback(resp);
        },
        [callback](const orm::DrogonDbException &e) {
          auto resp = HttpResponse::newHttpResponse();
          resp->setStatusCode(k500InternalServerError);
          resp->setBody("DB ERROR: " + std::string(e.base().what()));
          callback(resp);
        });
  });

  LOG_INFO << "Server running on: *:9007";
  app().run();
}
