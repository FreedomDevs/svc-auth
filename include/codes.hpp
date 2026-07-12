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
    static inline constexpr Code RESEND_SUCCESS{201, "RESEND_SUCCESS", "Email resend successfully"};
  };

  struct Error {
    using Code = Codes::Code;
    static inline constexpr Code AUTH_FAILED{401, "AUTH_FAILED", "Auth failed"};
    static inline constexpr Code NOT_FOUND{404, "NOT_FOUND", "Not found"};
    static inline constexpr Code USER_NOT_FOUND{404, "USER_NOT_FOUND", "user not found"};
    static inline constexpr Code INVALID_DATA{422, "INVALID_DATA", "invalid data"};
    static inline constexpr Code METHOD_INCORRECT{422, "METHOD_INCORRECT", "method incorrect"};

    static inline constexpr Code TOKEN_GENERATE_ERR{500, "TOKEN_GENERATE_ERR", "tokenb idi naxyi"};
    static inline constexpr Code JSON_INVALID{400, "JSON_INVALID", "Couldn't read json from response"};

    static inline constexpr Code PASSWORD_INVALID{401, "PASSWORD_INVALID", "Password invalid"};
    static inline constexpr Code REFRESH_TOKEN_INVALID{401, "REFRESH_TOKEN_INVALID", "Refresh token invalid"};
    static inline constexpr Code GAME_TOKEN_INVALID{401, "GAME_TOKEN_INVALID", "Game token invalid"};
    static inline constexpr Code USER_ALREADY_EXISTS{409, "USER_ALREADY_EXISTS", "User with this login already exists"};
    static inline constexpr Code HASHING_FAILED{500, "HASHING_FAILED", "Password hashing failed"};
    static inline constexpr Code USER_CREATION_FAILED{500, "USER_CREATION_FAILED", "Failed to create user in svc-users"};

    static inline constexpr Code TOO_MANY_ATTEMPTS{429, "TOO_MANY_ATTEMPTS", "Too many failed attempts"};
    static inline constexpr Code EMAIL_NOT_SET{401, "EMAIL_NOT_SET", "ИДИ НАХУЙ"};

    static inline constexpr Code INTERNAL_ERROR{500, "INTERNAL_ERROR", "Internal server error"};
  };
};
