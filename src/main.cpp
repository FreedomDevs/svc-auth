#include "config.hpp"
#include <drogon/drogon.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <trantor/utils/Logger.h>

#include "db/DBManager.hpp"

// Если прод то логи уровня info
// Если dev то Trace логи
#ifdef NDEBUG
#define LOG_LEVEL trantor::Logger::kInfo
#else
#define LOG_LEVEL trantor::Logger::kTrace
#endif

int main() {
  config::loadConfig();

  // Настраиваем буфер для stdout и stderr
  setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
  setvbuf(stderr, NULL, _IOLBF, BUFSIZ);

  // Выставляем уровень подробности логов
  drogon::app().setLogLevel(LOG_LEVEL);

  drogon::app().addListener(config::HOST, config::PORT);
  initDatabase();

  LOG_INFO << "Starting server on: " << config::HOST << ":" << config::PORT;
  drogon::app().run();
}
