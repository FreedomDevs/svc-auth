#include "services/minecraftTokenServices.hpp"
#include "services/uuidUtils.hpp"
#include <drogon/utils/Utilities.h>
#include <mutex>
#include <trantor/utils/Logger.h>
#include <unordered_map>
using namespace drogon;

std::unordered_map<uint64_t, Token> tokenIndex;
std::unordered_map<UUID, uint64_t> uuidIndex;

std::mutex globalMutex;

std::chrono::steady_clock::time_point lastGcTime{}; // нулевая дата

const auto MAX_TOKENS_BEFORE_GC = 500;

uint64_t generateToken() {
  uint64_t token = 0; // Создаём переменную с токеном

  // Маппим переменную token в массив байт
  uint8_t *bytes = reinterpret_cast<uint8_t *>(&token);

  // Заполняем наш массив данными (8 байт данных)
  utils::secureRandomBytes(bytes, 8);

  return token;
} // Интересный факт, комменты реально я писал, не гпт

uint64_t createTokenForUser(const UUID &userUUID) {
  std::unique_lock<std::mutex> lock(globalMutex);
  auto now = std::chrono::steady_clock::now();

  if (uuidIndex.count(userUUID)) {
    uint64_t old_token = uuidIndex[userUUID];

    if (now <= tokenIndex.at(old_token).expiry)
      LOG_WARN << "Пользоватесь " << userUUID.toString() << " сгенерировал новый токен, но он уже существовал, перегенерация";

    tokenIndex.erase(old_token);
    uuidIndex.erase(userUUID);
  }

  // Создаём переменную, TTL у нас 10 секунд
  Token token = Token(generateToken(), userUUID, now + std::chrono::seconds(10));
  uint64_t key = token.value;

  if (tokenIndex.size() > MAX_TOKENS_BEFORE_GC && now - lastGcTime > std::chrono::seconds(5))
    runTokenGC();

  tokenIndex.emplace(key, token);
  uuidIndex.emplace(userUUID, key);

  lock.unlock();
  return key;
}

bool popToken(const uint64_t &token, const UUID &userUUID) {
  std::unique_lock<std::mutex> lock(globalMutex);

  auto it = tokenIndex.find(token);
  if (it == tokenIndex.end()) {
    lock.unlock();
    LOG_WARN << "Попытка " << userUUID.toString() << " войти по токену, но он не существует";
    return false;
  }
  if (it->second.userUUID != userUUID) {
    lock.unlock();
    LOG_WARN << "Попытка " << userUUID.toString() << " войти по токену, но не принадлежит ему";
    return false;
  }

  // Прверяем TTL
  auto now = std::chrono::steady_clock::now();
  if (now > it->second.expiry) {
    tokenIndex.erase(it);
    uuidIndex.erase(userUUID);

    lock.unlock();
    LOG_WARN << "Попытка " << userUUID.toString() << " войти по токену, но он устарел";
    return false;
  }

  // Удаляем токен если он действителен, так как он одноразовый
  tokenIndex.erase(it);
  uuidIndex.erase(userUUID);
  return true;
}

// ДОЛЖЕН ВЫЗЫВАТСЯ ПОД МЬЮТЕКСОМ УЖЕ
void runTokenGC() {
  auto now = std::chrono::steady_clock::now();
  int deleted_count = 0;

  for (auto it = tokenIndex.begin(); it != tokenIndex.end();) {
    if (now > it->second.expiry) {
      uuidIndex.erase(it->second.userUUID);
      it = tokenIndex.erase(it);
      deleted_count++;
    } else {
      ++it;
    }
  }

  LOG_INFO << "Выполнен GC токенов игры, было удалено " << deleted_count << " токенов";
}
