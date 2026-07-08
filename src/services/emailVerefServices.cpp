#include "services/emailVerefServices.hpp"
#include "db/Repository.hpp"
#include "drogon/utils/coroutine.h"
#include "http/rabbitMqClient.hpp"
#include "http/usersClient.hpp"
#include "services/hashUtil.hpp"
#include "services/minecraftTokenServices.hpp"
#include <drogon/utils/Utilities.h>
#include <random>
#include <unordered_map>

namespace {
std::unordered_map<uint64_t, ConfirmationPandingEmailVereficationPending> data;

int genCode() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<int> dist(100000, 999999);

  return dist(gen);
}
} // namespace

drogon::Task<uint64_t> sendVereficationMail(ConfirmationPandingEmailVereficationPending dota2) {
  int code = genCode();
  uint64_t token = generateToken();
  std::chrono::steady_clock::time_point expiry = std::chrono::steady_clock::now() + std::chrono::minutes(15);

  dota2.code = code;
  dota2.expiry = expiry;

  data.emplace(token, dota2);

  co_await sendEmailRequest(dota2.email, dota2.login, std::to_string(dota2.code));

  if (ConfirmationPandingEmailVereficationPending::Type::Login != dota2.type) {
    std::string hashedPassword;
    try {
      hashedPassword = hashPassword(dota2.password);
    } catch (const std::exception &e) {
      throw std::runtime_error("Password hashing failed: " + std::string(e.what()));
    }

    dota2.password = hashedPassword;
  }

  co_return token;
};

drogon::Task<std::optional<ConfirmationPandingEmailVereficationPending>> verifyEmail(uint64_t token, int code,
                                                                                     std::optional<RegistrationPasskey> passkey) {
  if (!data.contains(token)) {
    co_return std::nullopt;
  };

  auto now = std::chrono::steady_clock::now();

  if (now >= data[token].expiry) {
    data.erase(token);
    co_return std::nullopt;
  }

  if (code != data[token].code) {
    co_return std::nullopt;
  }

  UsersClient usersClient;

  switch (data[token].type) {
  case (ConfirmationPandingEmailVereficationPending::Type::Register): {
    auto dota1 = data[token];

    // Создаём акк ебланчику чтобы не выткал
    auto createResult = co_await usersClient.createUser(dota1.login, dota1.password);
    if (std::holds_alternative<HttpError>(createResult)) {
      auto err = std::get<HttpError>(createResult);
      throw std::runtime_error("User creation failed: " + err.message);
    }

    UserResponseDto createdUser = std::get<UserResponseDto>(createResult);

    dota1.userId = UUID::fromString(createdUser.data.id);

    // Записываем ебланчику email чтобы не выткал х2
    bool emailSet = co_await Repository::IntegrationRepo::setEmail(dota1.userId.toString(), dota1.email);
    if (!emailSet) {
      throw std::runtime_error("failed to set email userID: " + dota1.userId.toString());
    }

    break;
  }
  case (ConfirmationPandingEmailVereficationPending::Type::Login): {
    auto satan = data[token];

    bool emailSet = co_await Repository::IntegrationRepo::setEmail(satan.userId.toString(), satan.email);
    if (!emailSet) {
      throw std::runtime_error("failed to set email userID: " + satan.userId.toString());
    }
    break;
  }
  }
  co_return data[token];
};

drogon::Task<void> resendEmail(uint64_t token) {
  if (!data.contains(token)) {
    co_return;
  };

  ConfirmationPandingEmailVereficationPending исус = data[token];

  int code = genCode();
  std::chrono::steady_clock::time_point expiry = std::chrono::steady_clock::now() + std::chrono::minutes(15);

  исус.code = code;
  исус.expiry = expiry;

  co_await sendEmailRequest(исус.email, исус.login, std::to_string(исус.code));
}
