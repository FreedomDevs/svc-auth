#pragma once
#include "db/Repository.hpp"
#include "drogon/HttpResponse.h"
#include <drogon/HttpController.h>

namespace RequestCheck {
struct ValidationError : public std::exception {
  drogon::HttpResponsePtr response;

  explicit ValidationError(drogon::HttpResponsePtr resp) : response(std::move(resp)) {}

  const char *what() const noexcept override { return "ValidationError"; }
};

const Json::Value *requireJson(const drogon::HttpRequestPtr &request);
std::string requireString(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field);
std::vector<char> requireBase64String(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field);
std::optional<std::string> requireStringOrNull(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field);
bool requireBool(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field, bool default_value = false);
drogon::Task<Repository::RefreshToken> requireRefreshToken(const drogon::HttpRequestPtr &request, const Json::Value &json,
                                                           const std::string &field);
void requireOneOf(const drogon::HttpRequestPtr &request, const std::string &field, const std::string &value,
                  const std::initializer_list<std::string> &allowed);

} // namespace RequestCheck
