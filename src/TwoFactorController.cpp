#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "drogon/HttpController.h"
#include "services/uuidUtils.hpp"
#include <cstdint>
#include <services/2faVerefService.hpp>
#include <string>

using namespace drogon;

class TwoFactorController : public HttpController<TwoFactorController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TwoFactorController::addTwoFactor, "/auth/twofa/add", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(TwoFactorController::get2FaCode, "/auth/twofa/get_code", Get, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(TwoFactorController::disable2Fa, "/auth/twofa/disable_2fa", Post, "TraceIdMiddleware", "LoggerMiddleware");
  METHOD_LIST_END

  Task<HttpResponsePtr> addTwoFactor(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string method = RequestCheck::requireString(request, *json, "method");
      int64_t code = RequestCheck::requireInt64(request, *json, "code");
      int64_t methodId = RequestCheck::requireInt64(request, *json, "methodId");

      Repository::TwoFAType type = Repository::twoFAFromString(method);
      Repository::IntegrationRepo integrations;

      switch (type) {
      case Repository::TwoFAType::Discord: {
        std::optional<UUID> uuid = getUuidWhithCode(code);
        if (uuid == std::nullopt) {
          co_return ResponseHandler::error(request, Codes::Error::NOT_FOUND);
        };

        bool setTg = co_await integrations.setDiscordId(uuid->toString(), methodId);
        if (!setTg) {
          co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
        }

        bool set2fa = co_await integrations.set2FA(uuid->toString(), Repository::TwoFAType::Discord);
        if (!set2fa) {
          co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
        }

        co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
        break;
      }

      case Repository::TwoFAType::Telegram: {
        std::optional<UUID> uuid = getUuidWhithCode(code);
        if (uuid == std::nullopt) {
          co_return ResponseHandler::error(request, Codes::Error::NOT_FOUND);
        };

        bool setTg = co_await integrations.setTelegramId(uuid->toString(), methodId);
        if (!setTg) {
          co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
        }

        bool set2fa = co_await integrations.set2FA(uuid->toString(), Repository::TwoFAType::Telegram);
        if (!set2fa) {
          co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
        }

        co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
        break;
      }

      case Repository::TwoFAType::None:
        co_return ResponseHandler::error(request, Codes::Error::INVALID_DATA);
        break;
      }

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::INTERNAL_ERROR);
    }
  }

  Task<HttpResponsePtr> get2FaCode(HttpRequestPtr request) {
    try {
      std::string type = request->getHeader("eauth-type");
      if (type != "user") {
        co_return ResponseHandler::error(request, "Eauth type not 'user'", Codes::Error::AUTH_FAILED);
      }

      std::string id = request->getHeader("eauth-user-id");
      if (id.empty()) {
        co_return ResponseHandler::error(request, "userid not exists", Codes::Error::INTERNAL_ERROR);
      }

      int code = generate2FaCode(UUID::fromString(id));

      Json::Value res;
      res["code"] = code;

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);

    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::INTERNAL_ERROR);
    }
  }

  Task<HttpResponsePtr> disable2Fa(HttpRequestPtr request) {
    try {
      std::string type = request->getHeader("eauth-type");
      if (type != "user") {
        co_return ResponseHandler::error(request, "Eauth type not 'user'", Codes::Error::AUTH_FAILED);
      }

      std::string id = request->getHeader("eauth-user-id");
      if (id.empty()) {
        co_return ResponseHandler::error(request, "userid not exists", Codes::Error::INTERNAL_ERROR);
      }

      Repository::IntegrationRepo integrations;
      bool disable2fa = co_await integrations.set2FA(id, Repository::TwoFAType::None);
      if (!disable2fa) {
        co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
      }

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::INTERNAL_ERROR);
    }
  }
};
