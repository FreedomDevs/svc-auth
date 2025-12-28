#include "drogon/HttpController.h"
#include "drogon/utils/Utilities.h"
#include <drogon/drogon.h>
#include <json/config.h>
#include <json/value.h>
#include <utility>
using namespace drogon;

class AuthController : public HttpController<AuthController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::registerEndpoint, "/auth/register", Post);
  METHOD_LIST_END

  void
  registerEndpoint(const HttpRequestPtr &request,
                   std::function<void(const HttpResponsePtr &)> &&callback) {
    LOG_INFO << "connected:" << (request->connected() ? "true" : "false");

    auto jsonPtr = request->jsonObject();
    if (jsonPtr == nullptr) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Couldn't read json from response");
      callback(resp);
      return;
    }

    auto login = jsonPtr->get("login", Json::nullValue);
    if (login == Json::nullValue || !login.isString()) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Login not defined or it is not string");
      callback(resp);
      return;
    }

    auto password = jsonPtr->get("password", Json::nullValue);
    if (password == Json::nullValue || !password.isString()) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Password not defined or it is not string");
      callback(resp);
      return;
    }

    Json::String loginString = login.asString();
    Json::String passwordString = password.asString();
    Json::Value res;
    std::vector<unsigned char> random(32);
    utils::secureRandomBytes(random.data(), random.size());
    res["refresh_token"] = utils::base64Encode(random.data(), random.size());

    auto resp = HttpResponse::newHttpJsonResponse(std::move(res));
    callback(resp);
  }
};
