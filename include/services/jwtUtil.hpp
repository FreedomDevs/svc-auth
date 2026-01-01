#pragma once

#include "services/uuidUtils.hpp"
#include <array>
#include <jwt-cpp/jwt.h>

struct AccessTokenData {
  UUID uuid;
  std::array<uint8_t, 32> tokenHash;
};

const std::string secret = "H6VUCBdEyX1bq9pwi1fgzMGCNuuFtGD8";
constexpr uint64_t ttlSeconds = 900;

/**
 * @brief Генерация access токена для пользователя
 * @param userId UUID пользователя
 * @param secret секретный ключ для подписи
 * @param ttlSeconds время жизни токена в секундах
 * @return JWT строка
 */
std::string generateAccessToken(const AccessTokenData &userId);

/**
 * @brief Проверка access токена
 * @param token JWT строка
 * @param secret секретный ключ для проверки подписи
 * @return UUID пользователя, если токен валиден, иначе std::nullopt
 */
std::optional<AccessTokenData> verifyAccessToken(const std::string &token);
