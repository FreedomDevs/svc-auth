#pragma once
#include "dto/userResponseDto.hpp"
#include "http/httpResult.hpp"
#include "httpClient.hpp"

class UsersClient {
public:
  UsersClient(const std::string &baseUrl = "http://localhost:9002") : http_(baseUrl) {}

  HttpResult<UserResponseDto> getUserById(std::string id) { return http_.get<UserResponseDto>("/users/" + id); }
  HttpResult<UserResponseDto> createUser(const std::string &login, const std::string &hashedPassword) {
    Json::Value body;
    body["name"] = login;
    body["password"] = hashedPassword;
    // Logs
    Json::StreamWriterBuilder writer;
    std::string jsonStr = Json::writeString(writer, body);
    std::cout << "[UsersClient::createUser] Payload: " << jsonStr << std::endl;

    return http_.post<Json::Value, UserResponseDto>("/users", body);
  }

private:
  HttpClient http_;
};
