#pragma once
#include "dto/userResponseDto.hpp"
#include "httpClient.hpp"

class UsersClient {
public:
  UsersClient(const std::string &baseUrl = "http://localhost:9002") : http_(baseUrl) {}

  HttpResult<UserResponseDto> getUserById(int id) { return http_.get<UserResponseDto>("/users/" + std::to_string(id)); }

private:
  HttpClient http_;
};
