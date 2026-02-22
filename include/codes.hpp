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
    static inline constexpr Code LIVE_OK{200, "LIVE_OK", "svc-auth жив"};

    static inline constexpr Code REGISTRATION_SUCCESS{201, "REGISTRATION_SUCCESS", "User successfully registered"};
  };

  struct Error {
    using Code = Codes::Code;
    static inline constexpr Code AUTH_FAILED{401, "AUTH_FAILED", "Auth failed"};
    static inline constexpr Code USER_NOT_FOUND{404, "USER_NOT_FOUND", "user not found"};
    static inline constexpr Code INVALID_DATA{422, "INVALID_DATA", "invalid data"};
    static inline constexpr Code METHOD_INCORRECT{422, "METHOD_INCORRECT", "method incorrect"};

    static inline constexpr Code TOKEN_GENERATE_ERR{500, "TOKEN_GENERATE_ERR", "tokenb idi naxyi"};
    static inline constexpr Code JSON_INVALID{400, "JSON_INVALID", "Couldn't read json from response"};

    static inline constexpr Code PASSWORD_INVALID{401, "PASSWORD_INVALID", "Password invalid"};
    static inline constexpr Code USER_ALREADY_EXISTS{409, "USER_ALREADY_EXISTS", "User with this login already exists"};
    static inline constexpr Code HASHING_FAILED{500, "HASHING_FAILED", "Password hashing failed"};
    static inline constexpr Code USER_CREATION_FAILED{500, "USER_CREATION_FAILED", "Failed to create user in svc-users"};
  };
};
