#pragma once
#include "metaDto.hpp"
#include "userDto.hpp"
#include <json/json.h>
#include <string>

struct UserResponseDto {
  UserDto data;
  std::string message;
  MetaDto meta;

  static UserResponseDto fromJson(const Json::Value &j) { return {UserDto::fromJson(j["data"]), j["message"].asString(), MetaDto::fromJson(j["meta"])}; }
};
