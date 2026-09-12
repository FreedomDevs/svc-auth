#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "drogon/HttpController.h"

using namespace drogon;

class TwoFactorController : public HttpController<TwoFactorController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(TwoFactorController::addTwoFactor, "/auth/twofa/add", Post, "TraceIdMiddleware", "LoggerMiddleware");
  METHOD_LIST_END

  Task<HttpResponsePtr> addTwoFactor(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string method = RequestCheck::requireString(request, *json, "method");
      std::string userId = RequestCheck::requireString(request, *json, "userId");
      int64_t telegramId = RequestCheck::requireInt64(request, *json, "telegramId");

      Repository::TwoFAType type = Repository::twoFAFromString(method);
      Repository::IntegrationRepo integrations;
      bool status;

      switch (type) {
      case Repository::TwoFAType::Discord:
        break;

      case Repository::TwoFAType::Telegram:
        status = integrations.setTelegramId(const std::string &userId, int64_t telegramId);
        break;

      case Repository::TwoFAType::None:
        co_return ResponseHandler::error(request, Codes::Error::INVALID_DATA);
        break;
      }

      if (!status) {
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
