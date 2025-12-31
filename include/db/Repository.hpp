#pragma once
#include <chrono>
#include <drogon/orm/Result.h>
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
  static Integration create(const std::string &userId);
  static std::optional<Integration> getByUserId(const std::string &userId);

  /* 2FA */
  static bool set2FA(const std::string &userId, TwoFAType type);

  /* Telegram */
  static bool setTelegramId(const std::string &userId, int64_t telegramId);
  static bool resetTelegramId(const std::string &userId);

  /* Discord */
  static bool setDiscordId(const std::string &userId, int64_t discordId);
  static bool resetDiscordId(const std::string &userId);

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
  static bool save(const std::string &userId, const std::string &tokenHash, int ttlSeconds);
  static std::optional<RefreshToken> getByHash(const std::string &tokenHash);
  static bool deleteByUserId(const std::string &userId);
  static bool deleteByHash(const std::string &tokenHash);
  static bool deleteExpiredByUserId(const std::string &userId);
  static bool deleteAllExpired();
};

} // namespace Repository
