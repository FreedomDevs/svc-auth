#include <drogon/drogon.h>
using namespace drogon;

class TraceIdMiddleware : public HttpMiddleware<TraceIdMiddleware> {
public:
  TraceIdMiddleware() {}; // do not omit constructor

  void invoke(const HttpRequestPtr &req, MiddlewareNextCallback &&nextCb, MiddlewareCallback &&mcb) override {
    std::string traceID = req->getHeader("X-Trace-Id");
    req->attributes()->insert("trace_id", traceID);

    // Do something before calling the next middleware
    nextCb([mcb = std::move(mcb), traceID](const HttpResponsePtr &resp) {
      // Do something after the next middleware returns
      resp->addHeader("X-Trace-Id", traceID);
      mcb(resp);
    });
  }
};
