#pragma once
#include <json/json.h>
#include <string>

struct MetaDto {
  std::string code;
  std::string traceId;
  std::string timestamp;

  static MetaDto fromJson(const Json::Value &j) { return {j["code"].asString(), j["traceId"].asString(), j["timestamp"].asString()}; }
};
