#pragma once
#include "ResponseHandler.hpp"
#include "codes.hpp"
#include "drogon/HttpResponse.h"
#include <drogon/HttpController.h>

namespace RequestCheck {

using Callback = std::function<void(const drogon::HttpResponsePtr &)>;

inline const Json::Value *requireJson(const drogon::HttpRequestPtr &request, Callback &callback) {
  auto json = request->jsonObject();
  if (!json) {
    callback(ResponseHandler::error(request, Codes::Error::JSON_INVALID));
    return nullptr;
  }
  return json.get();
}

inline bool requireString(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field, std::string &out, Callback &callback) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull() || !v.isString()) {
    callback(ResponseHandler::error(request, field + " not defined or it is not string", Codes::Error::INVALID_DATA));
    return false;
  }
  out = v.asString();
  return true;
}

inline bool requireOneOf(const drogon::HttpRequestPtr &request, const std::string &field, const std::string &value,
                         const std::initializer_list<std::string> &allowed, Callback &callback) {
  if (std::find(allowed.begin(), allowed.end(), value) == allowed.end()) {
    callback(ResponseHandler::error(request, field + " has incorrect value", Codes::Error::METHOD_INCORRECT));
    return false;
  }
  return true;
}

} // namespace RequestCheck
