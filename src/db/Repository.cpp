#include "db/Repository.hpp"
#include "db/DBManager.hpp"
#include <drogon/orm/Result.h>
#include <iostream>

using namespace Repository;

Integration IntegrationRepo::mapRowToIntegration(const drogon::orm::Row &row) {
  Integration i;
  i.userId = row["userid"].as<std::string>();

  if (!row["twofa"].isNull()) {
    try {
      i.twofa = twoFAFromString(row["twofa"].as<std::string>());
    } catch (const std::exception &e) {
      std::cerr << "Invalid twoFA value for userId=" << i.userId << ": " << e.what() << std::endl;
      i.twofa = TwoFAType::None;
    }
  }

  if (!row["discordid"].isNull())
    i.discordId = row["discordid"].as<int64_t>();
  if (!row["telegramid"].isNull())
    i.telegramId = row["telegramid"].as<int64_t>();
  return i;
}

/* INTEGRATIONS */

/**
 * Создаёт или возвращает существующую запись Integration для пользователя.
 * @param userId Идентификатор пользователя (UUID в виде строки)
 * @return Объект Integration, гарантированно с заполненным userId
 */
Integration IntegrationRepo::create(const std::string &userId) {
  if (userId.empty()) {
    std::cerr << "Invalid userId (empty string) in create()" << std::endl;
    Integration i;
    i.userId = userId;
    return i;
  }

  try {
    auto r = getDatabase()->execSqlSync("INSERT INTO integrations(userId) VALUES($1) "
                                        "ON CONFLICT(userId) DO UPDATE SET userId=EXCLUDED.userId "
                                        "RETURNING userId, twofa, discordId, telegramId",
                                        userId);

    if (!r.empty())
      return mapRowToIntegration(r[0]);

    Integration i;
    i.userId = userId;
    return i;

  } catch (const std::exception &e) {
    std::cerr << "Failed to create/get Integration for userId=" << userId << ": " << e.what() << std::endl;
    Integration i;
    i.userId = userId;
    return i;
  } catch (...) {
    std::cerr << "Unknown error in create() for userId=" << userId << std::endl;
    Integration i;
    i.userId = userId;
    return i;
  }
}

/**
 * Получает Integration пользователя по userId.
 * @param userId Идентификатор пользователя
 * @return std::optional<Integration> — объект Integration, если найден, иначе std::nullopt
 */
std::optional<Integration> IntegrationRepo::getByUserId(const std::string &userId) {
  if (userId.empty())
    return std::nullopt;

  try {
    auto r = getDatabase()->execSqlSync("SELECT userId, twofa, discordId, telegramId FROM integrations WHERE userId=$1", userId);

    if (r.empty())
      return std::nullopt;

    return mapRowToIntegration(r[0]);

  } catch (const std::exception &e) {
    std::cerr << "Failed to get Integration for userId=" << userId << ": " << e.what() << std::endl;
    return std::nullopt;
  } catch (...) {
    std::cerr << "Unknown error in getByUserId() for userId=" << userId << std::endl;
    return std::nullopt;
  }
}

/* 2FA */

/**
 * Устанавливает тип двухфакторной аутентификации для пользователя.
 * @param userId Идентификатор пользователя
 * @param type Тип двухфакторной аутентификации (None, Telegram, Discord)
 * @return true, если запись обновлена, false если пользователя нет или ошибка
 */
bool IntegrationRepo::set2FA(const std::string &userId, TwoFAType type) {
  if (userId.empty())
    return false;

  try {
    auto r = (type == TwoFAType::None) ? getDatabase()->execSqlSync("UPDATE integrations SET twofa=NULL WHERE userId=$1", userId)
                                       : getDatabase()->execSqlSync("UPDATE integrations SET twofa=$1 WHERE userId=$2", toString(type), userId);

    int affected = r.affectedRows();
    if (affected <= 0) {
      std::cerr << "User with userId=" << userId << " not found." << std::endl;
      return false;
    }
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to set 2FA for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error in set2FA() for userId=" << userId << std::endl;
    return false;
  }
}

/* Telegram */

/**
 * Устанавливает Telegram ID пользователя.
 * @param userId Идентификатор пользователя
 * @param telegramId Telegram ID (int64_t)
 * @return true, если запись обновлена, false если пользователя нет или ошибка
 */
bool IntegrationRepo::setTelegramId(const std::string &userId, int64_t telegramId) {
  if (userId.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("UPDATE integrations SET telegramId=$1 WHERE userId=$2", telegramId, userId);
    if (r.affectedRows() <= 0) {
      std::cerr << "User with userId=" << userId << " not found." << std::endl;
      return false;
    }
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to set Telegram ID for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error in setTelegramId() for userId=" << userId << std::endl;
    return false;
  }
}

/**
 * Сбрасывает Telegram ID пользователя.
 * @param userId Идентификатор пользователя
 * @return true, если запись обновлена, false если пользователя нет или ошибка
 */
bool IntegrationRepo::resetTelegramId(const std::string &userId) {
  if (userId.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("UPDATE integrations SET telegramId=NULL WHERE userId=$1", userId);
    if (r.affectedRows() <= 0) {
      std::cerr << "User with userId=" << userId << " not found." << std::endl;
      return false;
    }
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to reset Telegram ID for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error in resetTelegramId() for userId=" << userId << std::endl;
    return false;
  }
}

/* Discord */

/**
 * Устанавливает Discord ID пользователя.
 * @param userId Идентификатор пользователя
 * @param discordId Discord ID (int64_t)
 * @return true, если запись обновлена, false если пользователя нет или ошибка
 */
bool IntegrationRepo::setDiscordId(const std::string &userId, int64_t discordId) {
  if (userId.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("UPDATE integrations SET discordId=$1 WHERE userId=$2", discordId, userId);

    if (r.affectedRows() <= 0) {
      std::cerr << "User with userId=" << userId << " not found." << std::endl;
      return false;
    }
    return true;

  } catch (const std::exception &e) {
    std::cerr << "Failed to set Discord ID for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error in setDiscordId() for userId=" << userId << std::endl;
    return false;
  }
}

/**
 * Сбрасывает Discord ID пользователя.
 * @param userId Идентификатор пользователя
 * @return true, если запись обновлена, false если пользователя нет или ошибка
 */
bool IntegrationRepo::resetDiscordId(const std::string &userId) {
  if (userId.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("UPDATE integrations SET discordId=NULL WHERE userId=$1", userId);

    if (r.affectedRows() <= 0) {
      std::cerr << "User with userId=" << userId << " not found." << std::endl;
      return false;
    }
    return true;

  } catch (const std::exception &e) {
    std::cerr << "Failed to reset Discord ID for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error in resetDiscordId() for userId=" << userId << std::endl;
    return false;
  }
}

/* REFRESH TOKENS */

/**
 * Сохраняет refresh token в базе.
 * @param userId Идентификатор пользователя
 * @param tokenHash Хэш токена (строка или бинар)
 * @param ttlSeconds Время жизни токена в секундах
 * @return true, если токен успешно сохранён, false если ошибка или ttlSeconds <= 0
 */
bool RefreshTokenRepo::save(const std::string &userId, const std::string &tokenHash, int ttlSeconds) {
  if (userId.empty() || tokenHash.empty() || ttlSeconds <= 0)
    return false;

  try {
    getDatabase()->execSqlSync("INSERT INTO refresh_tokens(userId, tokenHash, expires_at) "
                               "VALUES ($1, $2, NOW() + ($3 || ' seconds')::interval)",
                               userId, tokenHash, ttlSeconds);
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Failed to save refresh token: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error in save() for userId=" << userId << std::endl;
    return false;
  }
}

/**
 * Получает refresh token по хэшу, если он ещё не истёк.
 * @param tokenHash Хэш токена
 * @return std::optional<RefreshToken> — объект RefreshToken, если найден, иначе std::nullopt
 */
std::optional<RefreshToken> RefreshTokenRepo::getByHash(const std::string &tokenHash) {
  if (tokenHash.empty())
    return std::nullopt;

  try {
    auto r = getDatabase()->execSqlSync("SELECT id, userId, tokenHash, expires_at "
                                        "FROM refresh_tokens "
                                        "WHERE tokenHash=$1 AND expires_at > NOW()",
                                        tokenHash);

    if (r.empty())
      return std::nullopt;

    RefreshToken t;
    t.id = r[0]["id"].as<int>();
    t.userId = r[0]["userid"].as<std::string>();
    t.tokenHash = r[0]["tokenhash"].as<std::string>();

    std::string tsStr = r[0]["expires_at"].as<std::string>();
    std::tm tm = {};
    std::istringstream ss(tsStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
      std::cerr << "Failed to parse expires_at timestamp: " << tsStr << std::endl;
      return std::nullopt;
    }

    t.expiresAt = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    return t;
  } catch (const std::exception &e) {
    std::cerr << "Failed to get refresh token: " << e.what() << std::endl;
    return std::nullopt;
  }
}

/**
 * Удаляет все refresh tokens пользователя.
 * @param userId Идентификатор пользователя
 * @return true, если хотя бы один токен удалён, false если пользователь не найден или ошибка
 */
bool RefreshTokenRepo::deleteByUserId(const std::string &userId) {
  if (userId.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("DELETE FROM refresh_tokens WHERE userId=$1", userId);
    return r.affectedRows() > 0;
  } catch (const std::exception &e) {
    std::cerr << "Failed to delete refresh tokens for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  }
}

/**
 * Удаляет конкретный refresh токен по хэшу.
 * @param tokenHash Хэш токена
 * @return true, если токен успешно удалён, false если токен не найден или произошла ошибка
 */
bool RefreshTokenRepo::deleteByHash(const std::string &tokenHash) {
  if (tokenHash.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("DELETE FROM refresh_tokens WHERE tokenHash=$1", tokenHash);
    return r.affectedRows() > 0;
  } catch (const std::exception &e) {
    std::cerr << "Failed to delete refresh token by hash: " << e.what() << std::endl;
    return false;
  }
}

/**
 * Удаляет все просроченные refresh токены конкретного пользователя.
 * @param userId Идентификатор пользователя
 * @return true, если удалён хотя бы один токен, false если токены не найдены или произошла ошибка
 */
bool RefreshTokenRepo::deleteExpiredByUserId(const std::string &userId) {
  if (userId.empty())
    return false;

  try {
    auto r = getDatabase()->execSqlSync("DELETE FROM refresh_tokens WHERE userId=$1 AND expires_at <= NOW()", userId);
    return r.affectedRows() > 0;
  } catch (const std::exception &e) {
    std::cerr << "Failed to delete expired refresh tokens for userId=" << userId << ": " << e.what() << std::endl;
    return false;
  }
}

/**
 * Удаляет все просроченные refresh токены из базы для всех пользователей.
 * @return true, если удалён хотя бы один токен, false если токены не найдены или произошла ошибка
 */
bool RefreshTokenRepo::deleteAllExpired() {
  try {
    auto r = getDatabase()->execSqlSync("DELETE FROM refresh_tokens WHERE expires_at <= NOW()");
    return r.affectedRows() > 0;
  } catch (const std::exception &e) {
    std::cerr << "Failed to delete all expired refresh tokens: " << e.what() << std::endl;
    return false;
  }
}
