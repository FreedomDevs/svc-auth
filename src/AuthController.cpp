#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "codes.hpp"
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
    handleLoginPassword(request, std::move(callback));
  }

  void loginEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
    handleLoginPassword(request, std::move(callback));
  }

  void refreshEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto &cb = callback;
    const Json::Value *json = RequestCheck::requireJson(request, cb);
    if (!json)
      return;

    std::string refreshToken;
    std::string method;

    if (!RequestCheck::requireString(request, *json, "refresh_token", refreshToken, cb))
      return;
    if (!RequestCheck::requireString(request, *json, "method", method, cb))
      return;
    if (!RequestCheck::requireOneOf(request, "method", method, {"Web", "Game"}, cb))
      return;

    UUID uuid = UUID::fromString("372d8631-754c-47d5-9465-4efa4fd3b0e5");

    Json::Value res;
    std::vector<char> refreshTokenData = utils::base64DecodeToVector(refreshToken);

    uint64_t token = createTokenForUser(uuid);
    uint8_t *bytes = reinterpret_cast<uint8_t *>(&token);
    res["token"] = utils::base64Encode(bytes, 8);

    callback(ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res));
  }

  void popGameTokenEndpoint(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto &cb = callback;
    const Json::Value *json = RequestCheck::requireJson(request, cb);
    if (!json)
      return;

    std::string gameToken;
    if (!RequestCheck::requireString(request, *json, "game_token", gameToken, cb))
      return;

    auto data = utils::base64DecodeToVector(gameToken);
    if (data.size() != sizeof(uint64_t)) {
      callback(ResponseHandler::error(request, "Token invalid format", Codes::Error::INVALID_DATA));
      return;
    }

    uint64_t token = 0;
    std::memcpy(&token, data.data(), sizeof(uint64_t));

    UUID uuid = UUID::fromString("372d8631-754c-47d5-9465-4efa4fd3b0e5");

    Json::Value res;
    res["result"] = popToken(token, uuid);

    callback(ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res));
  }

private:
  void handleLoginPassword(const HttpRequestPtr &request, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto &cb = callback;
    const Json::Value *json = RequestCheck::requireJson(request, cb);
    if (!json)
      return;

    std::string login;
    std::string password;

    if (!RequestCheck::requireString(request, *json, "login", login, cb))
      return;
    if (!RequestCheck::requireString(request, *json, "password", password, cb))
      return;

    Json::Value res;
    std::vector<unsigned char> random(32);
    utils::secureRandomBytes(random.data(), random.size());

    res["refresh_token"] = utils::base64Encode(random.data(), random.size());
    res["access_token"] = utils::base64Encode(random.data(), random.size());

    callback(ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res));
  }
};
