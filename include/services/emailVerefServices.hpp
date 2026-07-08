#pragma once
#include "drogon/utils/coroutine.h"
#include "services/uuidUtils.hpp"
#include <chrono>
#include <string>

struct ConfirmationPandingEmailVereficationPending {
  enum Type {
    Register,
    Login,
    Login_passkey,
    RefreshPassword,
  };

  Type type;

  UUID userId;
  std::string login;
  std::string email;
  std::string password;

  std::optional<std::array<char, 32>> passkey_challenge;

  int code;
  std::chrono::steady_clock::time_point expiry;
};

struct RegistrationPasskey {
  std::vector<char> id;
  std::vector<char> client_data;
  std::vector<char> attestation;
  std::vector<std::string> transports;
};

struct LoginRequest {
  std::string id;
  std::string client_data;
  std::string auth_data;
  std::string signature;
};

drogon::Task<uint64_t> sendVereficationMail(ConfirmationPandingEmailVereficationPending data);

drogon::Task<std::optional<ConfirmationPandingEmailVereficationPending>> verifyEmail(uint64_t token, int code,
                                                                                     std::optional<RegistrationPasskey> passkey);

drogon::Task<void> resendEmail(uint64_t token);
