#include "config.hpp"
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
}

int main() {
  config::loadConfig();

  // Магия для docker
  setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
  setvbuf(stderr, NULL, _IOLBF, BUFSIZ);

  // Выставляем уровень подробности логов
#ifdef NDEBUG
  app().setLogLevel(trantor::Logger::kInfo);
#else
  app().setLogLevel(trantor::Logger::kTrace);
#endif

  app().setBeforeListenSockOptCallback(preconfigurateSocket).addListener("::", 9007);
  initDatabase();

  LOG_INFO << "Server running on: *:9007";
  app().run();

  closeDatabase(); // Это никогда не вызовется из-за run, надо когда-то исправить и сделать чтобы оно типо закрывалось
}
