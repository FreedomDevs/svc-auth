#include <drogon/drogon.h>
#include <trantor/utils/Logger.h>
using namespace drogon;

class LoggerMiddleware : public HttpMiddleware<LoggerMiddleware> {
public:
  LoggerMiddleware() {};

  void invoke(const HttpRequestPtr &req, MiddlewareNextCallback &&nextCb, MiddlewareCallback &&mcb) override {
    auto start = std::chrono::steady_clock::now();
    auto traceID = req->attributes()->get<std::string>("trace_id");

    nextCb([=](const HttpResponsePtr &resp) {
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

      LOG_INFO << "HTTP " << req->getMethodString() << " " << req->path() << " -> " << resp->getStatusCode() << " " << ms << "ms"
               << " trace_id=" << traceID;
      mcb(resp);
    });
  }
};
