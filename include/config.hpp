#include <cstdint>
#include <string>

namespace config {
std::string getEnvOrDefault(const std::string &key, const std::string &defaultValue);
bool parseBoolSafe(const std::string &s);
int getEnvIntOrDefault(const std::string &key, int defaultValue);

inline uint32_t ARGON2_T_COST;
inline uint32_t ARGON2_M_COST;
inline uint32_t ARGON2_PARALLELISM;
inline uint32_t ARGON2_HASHLEN;
inline size_t ARGON2_SALT_LEN;

inline std::string JWT_SECRET;
inline int JWT_TTL_SECONDS;

inline size_t MAX_GAME_TOKENS_BEFORE_GC;

inline int DB_CONNECTIONS_POOL_SIZE;
inline std::string DB_CONNECT_STRING;

inline void loadConfig() {
  ARGON2_T_COST = getEnvIntOrDefault("ARGON2_T_COST", 2);       // Количество итераций
  ARGON2_M_COST = getEnvIntOrDefault("ARGON2_M_COST", 1 << 16); // 64 МБ памяти
  ARGON2_M_COST = getEnvIntOrDefault("ARGON2_PARALLELISM", 1);  // Число потоков
  ARGON2_M_COST = getEnvIntOrDefault("ARGON2_HASHLEN", 32);     // Длинна итогового хеша
  ARGON2_M_COST = getEnvIntOrDefault("ARGON2_SALT_LEN", 16);    // Длинна соли

  JWT_SECRET = getEnvOrDefault("JWT_SECRET", "H6VUCBdEyX1bq9pwi1fgzMGCNuuFtGD8");
  JWT_TTL_SECONDS = getEnvIntOrDefault("JWT_TTL_SECONDS", 15 * 60);

  MAX_GAME_TOKENS_BEFORE_GC = getEnvIntOrDefault("MAX_GAME_TOKENS_BEFORE_GC", 500);

  DB_CONNECTIONS_POOL_SIZE = getEnvIntOrDefault("DB_CONNECTIONS_POOL_SIZE", 3);
  DB_CONNECT_STRING = getEnvOrDefault("DB_CONNECT_STRING", "dbname=svc-auth user=postgres password=postgres host=127.0.0.1 port=8007");
}
} // namespace config
