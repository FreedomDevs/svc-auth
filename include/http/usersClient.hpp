#pragma once
#include "config.hpp"
#include "dto/userResponseDto.hpp"
#include "http/httpResult.hpp"
#include "httpClient.hpp"
#include "trantor/utils/Logger.h"

class UsersClient {
public:
  UsersClient() : http_(config::USER_SERVICE_URL) {}

  drogon::Task<HttpResult<UserResponseDto>> getUserById(std::string id, bool includePassword = false) {
    return http_.get<UserResponseDto>("/users/" + id + "?psw=" + (includePassword ? "true" : "false"));
  }
  drogon::Task<HttpResult<UserResponseDto>> createUser(const std::string &login, const std::string &hashedPassword) {
    Json::Value body;
    body["name"] = login;
    body["password"] = hashedPassword;
    // Logs
    Json::StreamWriterBuilder writer;
    std::string jsonStr = Json::writeString(writer, body);
    LOG_INFO << "[UsersClient::createUser] Payload: " << jsonStr;

    return http_.post<Json::Value, UserResponseDto>("/users", body);
  }

private:
  HttpClient http_;
};
