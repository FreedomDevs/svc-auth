#pragma once
#include "ResponseHandler.hpp"
#include "codes.hpp"
#include "drogon/HttpResponse.h"
#include <drogon/HttpController.h>

namespace RequestCheck {
struct ValidationError : public std::exception {
  drogon::HttpResponsePtr response;

  explicit ValidationError(drogon::HttpResponsePtr resp) : response(std::move(resp)) {}

  const char *what() const noexcept override { return "ValidationError"; }
};

inline const Json::Value *requireJson(const drogon::HttpRequestPtr &request) {
  auto json = request->jsonObject();
  if (!json)
    throw ValidationError(ResponseHandler::error(request, Codes::Error::JSON_INVALID));

  return json.get();
}

inline std::string requireString(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull() || !v.isString())
    throw ValidationError(ResponseHandler::error(request, field + " not defined or it is not string", Codes::Error::INVALID_DATA));

  return v.asString();
}

inline void requireOneOf(const drogon::HttpRequestPtr &request, const std::string &field, const std::string &value,
                         const std::initializer_list<std::string> &allowed) {
  if (std::find(allowed.begin(), allowed.end(), value) == allowed.end())
    throw ValidationError(ResponseHandler::error(request, field + " has incorrect value", Codes::Error::METHOD_INCORRECT));
}

} // namespace RequestCheck
