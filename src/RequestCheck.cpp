#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "codes.hpp"
#include "drogon/utils/Utilities.h"
#include "services/hashUtil.hpp"

namespace RequestCheck {

const Json::Value *requireJson(const drogon::HttpRequestPtr &request) {
  auto json = request->jsonObject();
  if (!json)
    throw ValidationError(ResponseHandler::error(request, Codes::Error::JSON_INVALID));

  return json.get();
}

std::string requireString(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull() || !v.isString())
    throw ValidationError(ResponseHandler::error(request, field + " not defined or it is not string", Codes::Error::INVALID_DATA));

  return v.asString();
}

std::vector<char> requireBase64String(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull() || !v.isString())
    throw ValidationError(ResponseHandler::error(request, field + " not defined or it is not string", Codes::Error::INVALID_DATA));

  std::string str = v.asString();

  try {
    return drogon::utils::base64DecodeToVector(str);
  } catch (std::exception e) {
    throw ValidationError(ResponseHandler::error(request, field + " not base64 string", Codes::Error::INVALID_DATA));
  }
}

std::optional<std::string> requireStringOrNull(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull())
    return std::nullopt;

  if (!v.isString())
    throw ValidationError(ResponseHandler::error(request, field + " is not string", Codes::Error::INVALID_DATA));

  return v.asString();
}

bool requireBool(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field, bool default_value) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull())
    return default_value;

  if (!v.isBool())
    throw ValidationError(ResponseHandler::error(request, field + " is not bool", Codes::Error::INVALID_DATA));

  return v.asBool();
}

#include "ResponseHandler.hpp"
#include "codes.hpp"
void requireOneOf(const drogon::HttpRequestPtr &request, const std::string &field, const std::string &value,
                  const std::initializer_list<std::string> &allowed) {
  if (std::find(allowed.begin(), allowed.end(), value) == allowed.end())
    throw ValidationError(ResponseHandler::error(request, field + " has incorrect value", Codes::Error::METHOD_INCORRECT));
}

drogon::Task<Repository::RefreshToken> requireRefreshToken(const drogon::HttpRequestPtr &request, const Json::Value &json,
                                                           const std::string &field) {
  std::string refreshToken = RequestCheck::requireString(request, json, field);
  if (refreshToken.size() != 44) {
    throw ValidationError(ResponseHandler::error(request, "Refresh token has invalid length", Codes::Error::REFRESH_TOKEN_INVALID));
  }

  std::string decoded = drogon::utils::base64Decode(refreshToken);
  if (decoded.size() != 32) {
    throw ValidationError(ResponseHandler::error(request, "Refresh token has invalid length", Codes::Error::REFRESH_TOKEN_INVALID));
  }

  std::vector<uint8_t> refreshData(32);
  std::copy(decoded.begin(), decoded.end(), refreshData.begin());
  auto refreshTokenHash = getRefreshTokenHash(refreshData);

  std::optional<Repository::RefreshToken> refreshTokenData = co_await Repository::RefreshTokenRepo::getByHash(refreshTokenHash);
  if (!refreshTokenData.has_value()) {
    throw ValidationError(ResponseHandler::error(request, "Refresh token invalid", Codes::Error::REFRESH_TOKEN_INVALID));
  }

  co_return std::move(*refreshTokenData);
}

} // namespace RequestCheck
