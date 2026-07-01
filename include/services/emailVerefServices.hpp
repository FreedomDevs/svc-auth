#pragma once
#include "drogon/utils/coroutine.h"
#include "services/uuidUtils.hpp"
#include <chrono>
#include <string>

struct ConfirmationPandingEmailVereficationPending {
  enum Type {
    Register,
    Login,
    RefreshPassword,
  };

  Type type;

  UUID userId;
  std::string login;
  std::string email;
  std::string password;

  int code;
  std::chrono::steady_clock::time_point expiry;
};

drogon::Task<uint64_t> sendVereficationMail(ConfirmationPandingEmailVereficationPending data);

drogon::Task<std::optional<ConfirmationPandingEmailVereficationPending>> verifyEmail(uint64_t token, int code);

drogon::Task<void> resendEmail(uint64_t token);
