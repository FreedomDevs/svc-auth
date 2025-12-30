#include "HealthCheckController.hpp"

#include "DBManager.hpp"
#include "ResponseHandler.hpp"
#include "codes.hpp"
#include <drogon/HttpController.h>
#include <exception>
#include <json/config.h>
#include <json/value.h>
using namespace drogon;

Task<HttpResponsePtr> healthEndpoint(HttpRequestPtr req) {
  Json::Value res;

  bool isWorking = true;
  try {
    auto result = co_await getDatabase()->execSqlCoro("SELECT 1;");
    if (result.size() == 0) {
      isWorking = false;
      LOG_WARN << "База вернула пустой результат";
    }
  } catch (const std::exception &ex) {
    LOG_ERROR << "Ошибка при обращении к базе: " << ex.what();
    isWorking = false;
  }

  if (isWorking) {
    res["details"]["database"] = "OK";
    res["ready"] = true;
    res["status"] = "OK";
  } else {
    res["details"]["database"] = "ERR";
    res["ready"] = false;
    res["status"] = "ERROR";
  }

  co_return ResponseHandler::success(req, Codes::Success::LIVE_OK, res);
}

Task<HttpResponsePtr> liveEndpoint(HttpRequestPtr req) {
  Json::Value res;
  res["alive"] = true;

  co_return ResponseHandler::success(req, Codes::Success::LIVE_OK, res);
}

void initHealthCheckController() {
  app()
      .registerHandler("/health", &healthEndpoint, {Get, "TraceIdMiddleware", "LoggerMiddleware"})
      .registerHandler("/live", &liveEndpoint, {Get, "TraceIdMiddleware", "LoggerMiddleware"});
}
