#pragma once
#include <chrono>
#include <drogon/orm/Result.h>
#include <drogon/utils/coroutine.h>
#include <optional>
#include <string>

namespace Repository {

enum class TwoFAType { None, Telegram, Discord };

inline std::string toString(TwoFAType type) {
  switch (type) {
  case TwoFAType::Telegram:
    return "tg";
  case TwoFAType::Discord:
    return "ds";
  case TwoFAType::None:
    return "";
  }
  throw std::logic_error("Unknown TwoFAType");
}

inline TwoFAType twoFAFromString(const std::string &v) {
  if (v == "tg")
    return TwoFAType::Telegram;
  if (v == "ds")
    return TwoFAType::Discord;
  return TwoFAType::None;
}

struct Integration {
  std::string userId;
  TwoFAType twofa = TwoFAType::None;
  std::optional<int64_t> discordId;
  std::optional<int64_t> telegramId;
};

class IntegrationRepo {
public:
  static drogon::Task<Integration> create(const std::string &userId);
  static drogon::Task<std::optional<Integration>> getByUserId(const std::string &userId);

  /* 2FA */
  static drogon::Task<bool> set2FA(const std::string &userId, TwoFAType type);

  /* Telegram */
  static drogon::Task<bool> setTelegramId(const std::string &userId, int64_t telegramId);
  static drogon::Task<bool> resetTelegramId(const std::string &userId);

  /* Discord */
  static drogon::Task<bool> setDiscordId(const std::string &userId, int64_t discordId);
  static drogon::Task<bool> resetDiscordId(const std::string &userId);

private:
  static Integration mapRowToIntegration(const drogon::orm::Row &row);
};

struct RefreshToken {
  int id;
  std::string userId;
  std::string tokenHash;
  std::chrono::system_clock::time_point expiresAt;
};

class RefreshTokenRepo {
public:
  static drogon::Task<bool> save(const std::string &userId, const std::string &tokenHash, int ttlSeconds);
  static drogon::Task<std::optional<RefreshToken>> getByHash(const std::string &tokenHash);
  static drogon::Task<bool> deleteByUserId(const std::string &userId);
  static drogon::Task<bool> deleteByHash(const std::string &tokenHash);
  static drogon::Task<bool> deleteExpiredByUserId(const std::string &userId);
  static drogon::Task<bool> deleteAllExpired();
};

} // namespace Repository
