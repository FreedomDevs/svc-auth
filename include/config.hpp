#pragma once
#include <cstdint>
#include <string>
#include <trantor/utils/Logger.h>

namespace config {
std::string getEnvOrDefault(const std::string &key, const std::string &defaultValue);
bool parseBoolSafe(const std::string &s);
int getEnvIntOrDefault(const std::string &key, int defaultValue);
std::string readFile(const std::string &filepath);

inline uint32_t ARGON2_T_COST;
inline uint32_t ARGON2_M_COST;
inline uint32_t ARGON2_PARALLELISM;
inline uint32_t ARGON2_HASHLEN;
inline size_t ARGON2_SALT_LEN;

inline std::string JWT_PRIV_KEY;
inline std::string JWT_PUB_KEY;
inline int JWT_TTL_SECONDS;

inline size_t MAX_GAME_TOKENS_BEFORE_GC;

inline int DB_CONNECTIONS_POOL_SIZE;
inline std::string DB_CONNECT_STRING;

inline std::string USER_SERVICE_URL;

inline void loadConfig() {
  ARGON2_T_COST = getEnvIntOrDefault("ARGON2_T_COST", 2);           // Количество итераций
  ARGON2_M_COST = getEnvIntOrDefault("ARGON2_M_COST", 1 << 16);     // 64 МБ памяти
  ARGON2_PARALLELISM = getEnvIntOrDefault("ARGON2_PARALLELISM", 1); // Число потоков
  ARGON2_HASHLEN = getEnvIntOrDefault("ARGON2_HASHLEN", 32);        // Длинна итогового хеша
  ARGON2_SALT_LEN = getEnvIntOrDefault("ARGON2_SALT_LEN", 16);      // Длинна соли

  JWT_PRIV_KEY = readFile("secrets/ed25519_private.pem");
  JWT_PUB_KEY = readFile("secrets/ed25519_public.pem");
  JWT_TTL_SECONDS = getEnvIntOrDefault("JWT_TTL_SECONDS", 15 * 60);

  MAX_GAME_TOKENS_BEFORE_GC = getEnvIntOrDefault("MAX_GAME_TOKENS_BEFORE_GC", 500);

  DB_CONNECTIONS_POOL_SIZE = getEnvIntOrDefault("DB_CONNECTIONS_POOL_SIZE", 3);
  DB_CONNECT_STRING = getEnvOrDefault("DB_CONNECT_STRING", "dbname=svc-auth user=postgres password=postgres host=postgres port=5432");

  // Это Docker gateway, потому что svc-users работает пока-что на хосте
  USER_SERVICE_URL = getEnvOrDefault("USER_SERVICE_URL", "http://[fd98:2dd6:8f48:1d99:22e6:f8c8::1]:9002");
}
} // namespace config
