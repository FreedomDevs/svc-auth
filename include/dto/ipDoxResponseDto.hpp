#pragma once
#include "ipDoxDto.hpp"
#include "metaDto.hpp"
#include <json/json.h>
#include <string>

struct IpDoxResponseDto {
  IpDoxDto data;
  std::string message;
  MetaDto meta;

  static IpDoxResponseDto fromJson(const Json::Value &j) {
    return {IpDoxDto::fromJson(j["data"]), j["message"].asString(), MetaDto::fromJson(j["meta"])};
  }
};
