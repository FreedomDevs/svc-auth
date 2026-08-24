#pragma once
#include <cstdint>
#include <string>
#include <trantor/utils/Logger.h>

namespace config {
std::string getEnvOrDefault(const std::string &key, const std::string &defaultValue);
bool parseBoolSafe(const std::string &s);
int getEnvIntOrDefault(const std::string &key, int defaultValue);
std::string readFile(const std::string &filepath);

inline std::string HOST = getEnvOrDefault("HOST", "::");
inline uint16_t PORT = getEnvIntOrDefault("PORT", 80);

inline uint32_t ARGON2_T_COST = getEnvIntOrDefault("ARGON2_T_COST", 2);           // Количество итераций
inline uint32_t ARGON2_M_COST = getEnvIntOrDefault("ARGON2_M_COST", 1 << 16);     // 64 МБ памяти
inline uint32_t ARGON2_PARALLELISM = getEnvIntOrDefault("ARGON2_PARALLELISM", 1); // Число потоков
inline uint32_t ARGON2_HASHLEN = getEnvIntOrDefault("ARGON2_HASHLEN", 32);        // Длинна итогового хеша
inline size_t ARGON2_SALT_LEN = getEnvIntOrDefault("ARGON2_SALT_LEN", 16);        // Длинна соли

inline std::string JWT_PRIV_KEY = readFile("secrets/ed25519_private.pem");
inline std::string JWT_PUB_KEY = readFile("secrets/ed25519_public.pem");
inline double JWT_TTL_SECONDS = (double)getEnvIntOrDefault("JWT_TTL_SECONDS", 15 * 60);

inline size_t MAX_GAME_TOKENS_BEFORE_GC = getEnvIntOrDefault("MAX_GAME_TOKENS_BEFORE_GC", 500);

inline int DB_CONNECTIONS_POOL_SIZE = getEnvIntOrDefault("DB_CONNECTIONS_POOL_SIZE", 3);
inline std::string DB_CONNECT_STRING =
    getEnvOrDefault("DB_CONNECT_STRING", "dbname=svc-auth user=postgres password=postgres host=fd98:2dd6:8f48:1d99:22e6:f8c8::3 port=5432");

inline std::string USER_SERVICE_URL = getEnvOrDefault("USER_SERVICE_URL", "http://[fd98:2dd6:8f48:1d99:dc28:e6e1::2]:80");
inline std::string DOX_SERVICE_URL = getEnvOrDefault("DOX_SERVICE_URL", "http://[fd98:2dd6:8f48:1d99:3b03:a78e::2]:80");

inline std::string RABBIT_MQ_URL = getEnvOrDefault("RABBIT_MQ_URL", "http://localhost:15672");
inline std::string RABBIT_MQ_LOGIN = getEnvOrDefault("RABBIT_MQ_LOGIN", "guest");
inline std::string RABBIT_MQ_PASSWORD = getEnvOrDefault("RABBIT_MQ_PASSWORD", "guest");
inline std::string RABBIT_MQ_QUEUE_NAME = getEnvOrDefault("RABBIT_MQ_QUEUE_NAME", "email_queue");
} // namespace config
