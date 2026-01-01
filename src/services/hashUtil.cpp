#include "services/hashUtil.hpp"
#include "drogon/utils/Utilities.h"
#include <stdexcept>

constexpr uint32_t ARGON2_T_COST = 2;       // количество итераций
constexpr uint32_t ARGON2_M_COST = 1 << 16; // 64 MB памяти
constexpr uint32_t ARGON2_PARALLELISM = 1;  // количество потоков
constexpr uint32_t ARGON2_HASHLEN = 32;     // длина хеша
constexpr size_t SALT_LEN = 16;

std::string hashPassword(const std::string &password) {
  // Буфер для хеша (в закодированном виде)
  char hash[128];

  std::array<unsigned char, SALT_LEN> salt;
  drogon::utils::secureRandomBytes(salt.data(), salt.size());

  // Генерируем хеш Argon2id
  int result = argon2id_hash_encoded(ARGON2_T_COST, ARGON2_M_COST, ARGON2_PARALLELISM, password.data(), password.size(), salt.data(), salt.size(),
                                     ARGON2_HASHLEN, hash, sizeof(hash));

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
