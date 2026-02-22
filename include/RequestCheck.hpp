#pragma once
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
void requireOneOf(const drogon::HttpRequestPtr &request, const std::string &field, const std::string &value,
                  const std::initializer_list<std::string> &allowed);

} // namespace RequestCheck
