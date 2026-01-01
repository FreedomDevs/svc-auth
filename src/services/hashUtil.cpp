#include "services/hashUtil.hpp"
#include "config.hpp"
#include "drogon/utils/Utilities.h"
#include <stdexcept>

std::string hashPassword(const std::string &password) {
  // Буфер для хеша (в закодированном виде)
  char hash[128];

  std::vector<uint8_t> salt(config::ARGON2_SALT_LEN);
  drogon::utils::secureRandomBytes(salt.data(), salt.size());

  // Генерируем хеш Argon2id
  int result = argon2id_hash_encoded(config::ARGON2_T_COST, config::ARGON2_M_COST, config::ARGON2_PARALLELISM, password.data(), password.size(), salt.data(),
                                     salt.size(), config::ARGON2_HASHLEN, hash, sizeof(hash));

  if (result != ARGON2_OK) {
    throw std::runtime_error(std::string("Argon2 hashing failed: ") + argon2_error_message(result));
  }

  return std::string(hash);
}

bool verifyPassword(const std::string &hash, const std::string &password) {
  int result = argon2id_verify(hash.c_str(), password.data(), password.size());

  if (result == ARGON2_OK) {
    return true; // пароль совпал
  } else if (result == ARGON2_VERIFY_MISMATCH) {
    return false; // пароль неверный
  } else {
    throw std::runtime_error(std::string("Argon2 verify failed: ") + argon2_error_message(result));
  }
}

std::array<uint8_t, 32> getRefreshTokenHash(const std::vector<uint8_t> &data) {
  std::array<uint8_t, 32> hash{};
  SHA256(data.data(), data.size(), hash.data());
  return hash;
}
