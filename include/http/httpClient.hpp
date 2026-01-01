#pragma once
#include "httpError.hpp"
#include "httpResult.hpp"
#include <chrono>
#include <drogon/drogon.h>
#include <string>
#include <type_traits>

class HttpClient {
public:
  HttpClient(std::string baseUrl, int retries = 3, std::chrono::milliseconds timeout = std::chrono::seconds(2))
      : client_(drogon::HttpClient::newHttpClient(baseUrl)), retries_(retries), timeout_(timeout) {}

  // GET
  template <typename Resp> HttpResult<Resp> get(const std::string &path) { return request<std::monostate, Resp>(drogon::Get, path, nullptr); }

  // POST
  template <typename Req, typename Resp> HttpResult<Resp> post(const std::string &path, const Req &body) {
    return request<Req, Resp>(drogon::Post, path, &body);
  }

  // PUT
  template <typename Req, typename Resp> HttpResult<Resp> put(const std::string &path, const Req &body) { return request<Req, Resp>(drogon::Put, path, &body); }

  // DELETE
  HttpResult<std::monostate> del(const std::string &path) { return request<std::monostate, std::monostate>(drogon::Delete, path, nullptr); }

private:
  template <typename Req, typename Resp> HttpResult<Resp> request(drogon::HttpMethod method, const std::string &path, const Req *body) {
    const double timeoutSec = std::chrono::duration_cast<std::chrono::duration<double>>(timeout_).count();

    for (int attempt = 1; attempt <= retries_; ++attempt) {
      auto req = drogon::HttpRequest::newHttpRequest();
      req->setMethod(method);
      req->setPath(path);

      if constexpr (!std::is_same_v<Req, std::monostate>) {
        req->addHeader("Content-Type", "application/json");
        try {
          Json::Value json = body->toJson();
          req->setBody(json.toStyledString());
        } catch (const std::exception &e) {
          return HttpError{HttpErrorType::Serialization, 0, e.what()};
        }
      }

      auto [result, resp] = client_->sendRequest(req, timeoutSec);

      if (result != drogon::ReqResult::Ok) {
        if (attempt == retries_) {
          return HttpError{HttpErrorType::Network, 0, "HTTP request failed (network/timeout)"};
        }
        continue;
      }

      if (resp->getStatusCode() >= 300) {
        return HttpError{HttpErrorType::HttpStatus, resp->getStatusCode(), std::string(resp->getBody())};
      }

      if constexpr (std::is_same_v<Resp, std::monostate>) {
        return Resp{};
      } else {
        auto json = resp->getJsonObject();
        if (!json) {
          return HttpError{HttpErrorType::Deserialization, resp->getStatusCode(), "Invalid JSON response"};
        }

        return Resp::fromJson(*json);
      }
    }

    return HttpError{HttpErrorType::Network, 0, "Unreachable"};
  }

private:
  drogon::HttpClientPtr client_;
  int retries_;
  std::chrono::milliseconds timeout_;
};
