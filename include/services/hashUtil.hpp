#pragma once
#include <argon2.h>
#include <openssl/sha.h>
#include <string>
#include <vector>

/**
 * @brief Хеширует пароль с использованием Argon2id
 * @param password Пароль в виде строки
 * @return закодированный хеш (включает соль и параметры)
 */
std::string hashPassword(const std::string &password);

/**
 * @brief Проверяет пароль на соответствие хешу
 * @param hash Хранится в базе данных (результат hashPassword)
 * @param password Пароль, который нужно проверить
 * @return true если пароль верный, false иначе
 */
bool verifyPassword(const std::string &hash, const std::string &password);

/**
 * @brief Генерирует хеш для refresh токена
 * @param refreshToken сам собственно refresh токен
 * @return Массив из 32 uint8_t (sha256 хеш)
 */
std::array<uint8_t, 32> getRefreshTokenHash(const std::vector<uint8_t> &refreshToken);
