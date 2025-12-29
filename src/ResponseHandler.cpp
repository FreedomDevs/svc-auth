#include "ResponseHandler.hpp"
#include "drogon/HttpResponse.h"
#include <drogon/HttpTypes.h>

// Публичные методы

drogon::HttpResponsePtr ResponseHandler::success(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code, const Json::Value &data) {
  return successImpl(req, message, code, data);
}

drogon::HttpResponsePtr ResponseHandler::success(const drogon::HttpRequestPtr &req, Codes::Code code, const Json::Value &data) {
  return successImpl(req, std::string(code.message), code, data);
}

drogon::HttpResponsePtr ResponseHandler::error(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code) {
  return errorImpl(req, message, code);
}

drogon::HttpResponsePtr ResponseHandler::error(const drogon::HttpRequestPtr &req, Codes::Code code) { return errorImpl(req, std::string(code.message), code); }

// Реализация error и success

drogon::HttpResponsePtr ResponseHandler::successImpl(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code, const Json::Value &data) {
  Json::Value content;
  content["data"] = data;
  content["message"] = message;

  content["meta"]["traceId"] = req->attributes()->get<std::string>("trace_id");
  content["meta"]["code"] = std::string(code.string);
  content["meta"]["timestamp"] = currentTime();

  auto resp = drogon::HttpResponse::newHttpJsonResponse(content);
  resp->setStatusCode(static_cast<drogon::HttpStatusCode>(code.code));

  return resp;
}

drogon::HttpResponsePtr ResponseHandler::errorImpl(const drogon::HttpRequestPtr &req, const std::string &message, Codes::Code code) {
  Json::Value content;
  content["error"]["message"] = message;
  content["error"]["code"] = std::string(code.string);

  content["meta"]["traceId"] = req->attributes()->get<std::string>("trace_id");
  content["meta"]["timestamp"] = currentTime();

  auto resp = drogon::HttpResponse::newHttpJsonResponse(content);
  resp->setStatusCode(static_cast<drogon::HttpStatusCode>(code.code));

  return resp;
}

std::string ResponseHandler::currentTime() {
  auto now = std::chrono::system_clock::now();
  auto t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(gmtime(&t), "%FT%TZ");
  return ss.str();
}
