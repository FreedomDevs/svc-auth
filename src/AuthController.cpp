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
  ADD_METHOD_TO(AuthController::loginEndpoint, "/auth/login", Post);
  ADD_METHOD_TO(AuthController::refreshEndpoint, "/auth/refresh", Post);
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
    res["access_token"] = utils::base64Encode(random.data(), random.size());

    auto resp = HttpResponse::newHttpJsonResponse(std::move(res));
    callback(resp);
  }

  void loginEndpoint(const HttpRequestPtr &request,
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
    res["access_token"] = utils::base64Encode(random.data(), random.size());

    auto resp = HttpResponse::newHttpJsonResponse(std::move(res));
    callback(resp);
  }

  void
  refreshEndpoint(const HttpRequestPtr &request,
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

    auto refreshToken = jsonPtr->get("refresh_token", Json::nullValue);
    if (refreshToken == Json::nullValue || !refreshToken.isString()) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Refresh token not defined or it is not string");
      callback(resp);
      return;
    }

    auto method = jsonPtr->get("method", Json::nullValue);
    if (method == Json::nullValue || !method.isString()) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Method not defined or it is not string");
      callback(resp);
      return;
    }

    Json::String refreshTokenString = refreshToken.asString();
    Json::String methodString = method.asString();
    if (method != "Web" && method != "Game") {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Method incorrect");
      callback(resp);
      return;
    }

    Json::Value res;
    std::vector<char> refreshTokenData =
        utils::base64DecodeToVector(refreshTokenString);

    std::vector<unsigned char> random(8);
    utils::secureRandomBytes(random.data(), random.size());
    res["token"] = utils::base64Encode(random.data(), random.size());

    auto resp = HttpResponse::newHttpJsonResponse(std::move(res));
    callback(resp);
  }
};
