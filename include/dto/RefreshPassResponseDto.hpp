#pragma once
#include "metaDto.hpp"
#include <json/json.h>
#include <string>

struct refreshDto {
  std::string password;

  static refreshDto fromJson(const Json::Value &j) {
    refreshDto refresh;
    refresh.password = j["password"].asString();
    return refresh;
  }
};

struct RefreshPassResponseDto {
  refreshDto data;
  std::string message;
  MetaDto meta;

  static RefreshPassResponseDto fromJson(const Json::Value &j) {
    return {refreshDto::fromJson(j["data"]), j["message"].asString(), MetaDto::fromJson(j["meta"])};
  }
};
