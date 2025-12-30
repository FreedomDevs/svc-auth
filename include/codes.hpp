#pragma once

#include <string_view>

struct Codes {
  struct Code {
    int code;
    std::string_view string;
    std::string_view message;
  };

  struct Success {
    using Code = Codes::Code;
    static inline constexpr Code AUTH_SUCCESS{200, "AUTH_SUCCESS", "Auth Success"};
  };

  struct Error {
    using Code = Codes::Code;
    static inline constexpr Code AUTH_FAILED{401, "AUTH_FAILED", "Auth failed"};
    static inline constexpr Code USER_NOT_FOUND{404, "USER_NOT_FOUND", "user not found"};
    static inline constexpr Code INVALID_DATA{422, "INVALID_DATA", "invalid data"};
    static inline constexpr Code METHOD_INCORRECT{422, "METHOD_INCORRECT", "method incorrect"};

    static inline constexpr Code TOKEN_GENERATE_ERR{500, "TOKEN_GENERATE_ERR", "tokenb idi naxyi"};
    static inline constexpr Code JSON_INVALID{400, "JSON_INVALID", "Couldn't read json from response"};
  };
};
