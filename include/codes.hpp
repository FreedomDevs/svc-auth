#pragma once

#include <string_view>

struct Codes {
  enum class Code { AUTH_SUCCESS, AUTH_FAILED, USER_NOT_FOUND, TOKEN_GENERATE_ERR };

  static inline constexpr std::string_view to_string(Code code) {
    switch (code) {
    case Code::AUTH_SUCCESS:
      return "AUTH_SUCCESS";
    case Code::AUTH_FAILED:
      return "AUTH_FAILED";
    case Code::USER_NOT_FOUND:
      return "USER_NOT_FOUND";
    case Code::TOKEN_GENERATE_ERR:
      return "TOKEN_GENERATE_ERR";
    }
    return "UNKNOWN_CODE";
  }
};
