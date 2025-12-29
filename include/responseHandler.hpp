#pragma once

#include "codes.hpp"
#include <drogon/HttpResponse.h>
#include <json/json.h>

class ResponseHandler {
public:
  static drogon::HttpResponsePtr success(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code,
                                         const Json::Value &data = Json::nullValue);

  static drogon::HttpResponsePtr success(const drogon::HttpRequestPtr &req, Codes::Code code, const Json::Value &data = Json::nullValue);

  static drogon::HttpResponsePtr error(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code);
  static drogon::HttpResponsePtr error(const drogon::HttpRequestPtr &req, Codes::Code code);

private:
  static drogon::HttpResponsePtr successImpl(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code, const Json::Value &data);
  static drogon::HttpResponsePtr errorImpl(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code);
  static std::string currentTime();
};
