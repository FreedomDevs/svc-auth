#pragma once

#include "codes.hpp"
#include <drogon/HttpResponse.h>
#include <json/json.h>

class ResponseHandler {
public:
  static drogon::HttpResponsePtr success(int statusCode, const std::string &message, Codes::Code code, const Json::Value &data = Json::nullValue);
  static drogon::HttpResponsePtr error(int statusCode, const std::string &message, Codes::Code code);
};
