#pragma once
#include "drogon/HttpResponse.h"
#include "httpError.hpp"
#include "httpResult.hpp"
#include <chrono>
#include <drogon/drogon.h>
#include <json/writer.h>
#include <string>
#include <type_traits>

class HttpClient {
public:
  HttpClient(std::string baseUrl, int retries = 3, std::chrono::milliseconds timeout = std::chrono::seconds(2))
      : client_(drogon::HttpClient::newHttpClient(baseUrl)), retries_(retries), timeout_(timeout) {}

  template <typename Resp> drogon::Task<HttpResult<Resp>> get(const std::string &path) {
    return request<std::monostate, Resp>(drogon::Get, path, std::monostate{});
  }

  template <typename Req, typename Resp> drogon::Task<HttpResult<Resp>> post(const std::string &path, const Req &body) {
    return request<Req, Resp>(drogon::Post, path, body);
  }

  template <typename Req, typename Resp> drogon::Task<HttpResult<Resp>> put(const std::string &path, const Req &body) {
    return request<Req, Resp>(drogon::Put, path, body);
  }

  drogon::Task<HttpResult<std::monostate>> del(const std::string &path) {
    return request<std::monostate, std::monostate>(drogon::Delete, path, std::monostate{});
  }

private:
  template <typename Req, typename Resp> drogon::Task<HttpResult<Resp>> request(drogon::HttpMethod method, std::string path, Req body) {
    try {
      const double timeoutSec = std::chrono::duration_cast<std::chrono::duration<double>>(timeout_).count();

      for (int attempt = 1; attempt <= retries_; ++attempt) {
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(method);
        req->setPath(path);

        LOG_INFO << "3";
        if constexpr (!std::is_same_v<Req, std::monostate>) {
          req->setContentTypeCode(drogon::CT_APPLICATION_JSON);

          if (!body) {
            co_return HttpError{HttpErrorType::Serialization, 0, "Body pointer is null"};
          }

          Json::StreamWriterBuilder builder;
          builder["indentation"] = ""; // Гарантируем отсутствие отступов

          // Сериализуем Json::Value безопасно
          req->setBody(Json::writeString(builder, body));
        }
        LOG_INFO << "4";

        drogon::HttpResponsePtr resp = co_await client_->sendRequestCoro(req, timeoutSec);

        if (resp->getStatusCode() >= 300) {
          co_return HttpError{HttpErrorType::HttpStatus, resp->getStatusCode(), std::string(resp->getBody())};
        }

        if constexpr (std::is_same_v<Resp, std::monostate>) {
          co_return Resp{};
        } else {
          auto json = resp->getJsonObject();
          if (!json) {
            co_return HttpError{HttpErrorType::Deserialization, resp->getStatusCode(), "Invalid JSON response"};
          }

          co_return Resp::fromJson(*json);
        }
      }

      co_return HttpError{HttpErrorType::Network, 0, "Unreachable"};
    } catch (const std::exception &e) {
      co_return HttpError{HttpErrorType::Network, 0, "HTTP request failed (network/timeout)"};
    }
  }

private:
  drogon::HttpClientPtr client_;
  int retries_;
  std::chrono::milliseconds timeout_;
};
