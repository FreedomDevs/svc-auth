#pragma once
#include <argon2.h>
#include <string>

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
