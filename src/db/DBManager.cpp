#include "db/DBManager.hpp"
#include "config.hpp"
#include <string>

drogon::orm::DbClientPtr client = nullptr;

const std::string INTEGRATIONS_TABLE = R"(
CREATE TABLE IF NOT EXISTS integrations (
  userId UUID PRIMARY KEY,
  twofa VARCHAR(10) DEFAULT NULL,
  discordId BIGINT DEFAULT NULL,
  telegramId BIGINT DEFAULT NULL,
  email TEXT DEFAULT NULL
);
)";
const std::string REFRESH_TOKENS_TABLE = R"(
CREATE TABLE IF NOT EXISTS refresh_tokens (
  tokenHash BYTEA PRIMARY KEY NOT NULL,
  userId UUID NOT NULL,
  description TEXT,
  expires_at TIMESTAMP NOT NULL
);
)";
const std::string CLIENT_INFO_TABLE = R"(
CREATE TABLE IF NOT EXISTS clients_info (
  id TEXT PRIMARY KEY,
  name TEXT NOT NULL,
  description TEXT NOT NULL,
  redirect_url TEXT NOT NULL
)
)";

const std::string TOKEN_HASH_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS idx_refresh_tokens_tokenhash ON refresh_tokens(tokenHash);";
const std::string TOKEN_EXPIRES_AT_INDEX = "CREATE INDEX IF NOT EXISTS idx_refresh_tokens_expires_at ON refresh_tokens(expires_at);";

void initDatabase() {
  client = drogon::orm::DbClient::newPgClient(config::DB_CONNECT_STRING, config::DB_CONNECTIONS_POOL_SIZE);
  client->execSqlSync(INTEGRATIONS_TABLE);
  client->execSqlSync(REFRESH_TOKENS_TABLE);
  client->execSqlSync(TOKEN_HASH_INDEX);
  client->execSqlSync(TOKEN_EXPIRES_AT_INDEX);

  client->execSqlSync(CLIENT_INFO_TABLE);
}
void closeDatabase() { client = nullptr; }

drogon::orm::DbClientPtr getDatabase() { return client; }
