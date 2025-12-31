#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "codes.hpp"
#include "drogon/HttpResponse.h"
#include "services/minecraftTokenServices.hpp"
#include "services/uuidUtils.hpp"
#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <endian.h>
#include <json/config.h>
#include <json/value.h>
using namespace drogon;

class AuthController : public HttpController<AuthController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::registerEndpoint, "/auth/register", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::loginEndpoint, "/auth/login", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::refreshEndpoint, "/auth/refresh", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::popGameTokenEndpoint, "/auth/pop_game_token", Post, "TraceIdMiddleware", "LoggerMiddleware");
  METHOD_LIST_END

  Task<HttpResponsePtr> registerEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string login = RequestCheck::requireString(request, *json, "login");
      std::string password = RequestCheck::requireString(request, *json, "password");

      Json::Value res;
      std::vector<unsigned char> random(32);
      utils::secureRandomBytes(random.data(), random.size());

      res["refresh_token"] = utils::base64Encode(random.data(), random.size());
      res["access_token"] = utils::base64Encode(random.data(), random.size());

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    }
  }

  Task<HttpResponsePtr> loginEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string login = RequestCheck::requireString(request, *json, "login");
      std::string password = RequestCheck::requireString(request, *json, "password");

      std::vector<unsigned char> random(32);
      utils::secureRandomBytes(random.data(), random.size());

      Json::Value res;
      res["refresh_token"] = utils::base64Encode(random.data(), random.size());
      res["access_token"] = utils::base64Encode(random.data(), random.size());

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    }
  }

  Task<HttpResponsePtr> refreshEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string refreshToken = RequestCheck::requireString(request, *json, "refresh_token");
      std::string method = RequestCheck::requireString(request, *json, "method");
      RequestCheck::requireOneOf(request, "method", method, {"Web", "Game"});

      UUID uuid = UUID::fromString("372d8631-754c-47d5-9465-4efa4fd3b0e5");

      std::vector<char> refreshTokenData = utils::base64DecodeToVector(refreshToken);

      uint64_t token = createTokenForUser(uuid);
      uint8_t *bytes = reinterpret_cast<uint8_t *>(&token);

      Json::Value res;
      res["token"] = utils::base64Encode(bytes, 8);

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    }
  }

  Task<HttpResponsePtr> popGameTokenEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string gameToken = RequestCheck::requireString(request, *json, "game_token");

      auto data = utils::base64DecodeToVector(gameToken);
      if (data.size() != sizeof(uint64_t))
        co_return ResponseHandler::error(request, "Token invalid format", Codes::Error::INVALID_DATA);

      uint64_t token = 0;
      std::memcpy(&token, data.data(), sizeof(uint64_t));

      UUID uuid = UUID::fromString("372d8631-754c-47d5-9465-4efa4fd3b0e5");

      bool result = popToken(token, uuid);
      Json::Value res;
      res["result"] = result;

      if (result)
        co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
      else
        co_return ResponseHandler::error(request, Codes::Error::AUTH_FAILED);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    }
  }
};
