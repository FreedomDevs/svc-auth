#include "services/jwtUtil.hpp"
#include "services/uuidUtils.hpp"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <trantor/utils/Logger.h>

#include <drogon/drogon.h>
using namespace drogon;

#include "db/DBManager.hpp"

void preconfigurateSocket(int fd) {
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
  config::loadConfig();

  AccessTokenData token{UUID::fromString("71b911d4-14bc-4f5b-b66a-477974c4de0d"),
                        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

  auto tokean = generateAccessToken(token);
  LOG_INFO << verifyAccessToken(tokean).has_value();

  app().setBeforeListenSockOptCallback(preconfigurateSocket).addListener("::", 9007);
  initDatabase();

  LOG_INFO << "Server running on: *:9007";
  app().run();

  closeDatabase(); // Это никогда не вызовется из-за run, надо когда-то исправить и сделать чтобы оно типо закрывалось
}
