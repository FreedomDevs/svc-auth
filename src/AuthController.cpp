#include "services/minecraftTokenServices.hpp"
#include "services/uuidUtils.hpp"
#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <endian.h>
#include <json/config.h>
#include <json/value.h>
#include <utility>
using namespace drogon;

class AuthController : public HttpController<AuthController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::registerEndpoint, "/auth/register", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::loginEndpoint, "/auth/login", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::refreshEndpoint, "/auth/refresh", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::popGameTokenEndpoint, "/auth/pop_game_token", Post, "TraceIdMiddleware", "LoggerMiddleware");
  METHOD_LIST_END

  void registerEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
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

  void loginEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
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

  void refreshEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
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

    UUID uuid = UUID::fromString("372d8631-754c-47d5-9465-4efa4fd3b0e5");

    Json::Value res;
    std::vector<char> refreshTokenData = utils::base64DecodeToVector(refreshTokenString);

    uint64_t token = createTokenForUser(uuid);
    uint8_t *bytes = reinterpret_cast<uint8_t *>(&token);
    res["token"] = utils::base64Encode(bytes, 8);

    auto resp = HttpResponse::newHttpJsonResponse(std::move(res));
    callback(resp);
  }

  void popGameTokenEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto jsonPtr = request->jsonObject();
    if (jsonPtr == nullptr) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Couldn't read json from response");
      callback(resp);
      return;
    }

    auto gameToken = jsonPtr->get("game_token", Json::nullValue);
    if (gameToken == Json::nullValue || !gameToken.isString()) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Refresh token not defined or it is not string");
      callback(resp);
      return;
    }

    std::vector<char> gameTokenVector = utils::base64DecodeToVector(gameToken.asString());
    if (gameTokenVector.size() != 8) {
      auto resp = HttpResponse::newHttpResponse();
      // #TODO нормальный формат ответов
      resp->setBody("Token invalid format, length ");
      callback(resp);
      return;
    }

    uint64_t token = 0;
    std::memcpy(&token, gameTokenVector.data(), sizeof(uint64_t));

    UUID uuid = UUID::fromString("372d8631-754c-47d5-9465-4efa4fd3b0e5");

    Json::Value res;
    res["result"] = popToken(token, uuid);

    auto resp = HttpResponse::newHttpJsonResponse(std::move(res));
    callback(resp);
  }
};
