#pragma once
#include <json/json.h>
#include <string>
#include <vector>

struct UserDto {
  std::string id;
  std::string name;
  std::vector<std::string> roles;
  std::string createdAt;
  std::string updatedAt;

  static UserDto fromJson(const Json::Value &j) {
    UserDto user;
    user.id = j["id"].asString();
    user.name = j["name"].asString();

    if (j["roles"].isArray()) {
      for (const auto &r : j["roles"]) {
        user.roles.push_back(r.asString());
      }
    }

    user.createdAt = j["createdAt"].asString();
    user.updatedAt = j["updatedAt"].asString();
    return user;
  }
};
