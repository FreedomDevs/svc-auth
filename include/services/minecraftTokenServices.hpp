#pragma once

#include "uuidUtils.hpp"
#include <chrono>
#include <cstdint>

struct Token {
  std::string username;                         // Ник игрока
  uint64_t value;                               // сам токен
  UUID userUUID;                                // UUID пользователя
  std::chrono::steady_clock::time_point expiry; // время жизни токена
};

uint64_t generateToken();
uint64_t createTokenForUser(const UUID &userUUID);
std::optional<Token> popToken(const uint64_t &token);
void runTokenGC();
