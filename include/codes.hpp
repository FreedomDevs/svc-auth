#pragma once

#include <string_view>

enum class Codes { AUTH_SUCCESS, AUTH_FAILED, USER_NOT_FOUND, TOKEN_GENERATE_ERR };

inline constexpr std::string_view to_string(Codes code) {
  switch (code) {
  case Codes::AUTH_SUCCESS:
    return "AUTH_SUCCESS";
  case Codes::AUTH_FAILED:
    return "AUTH_FAILED";
  case Codes::USER_NOT_FOUND:
    return "USER_NOT_FOUND";
  case Codes::TOKEN_GENERATE_ERR:
    return "TOKEN_GENERATE_ERR";
  }
  return "UNKNOWN_CODE";
}
