#include "DBManager.hpp"
#include <string>

const std::string DB_CONNECT_STRING = "dbname=svc-auth user=postgres password=postgres host=127.0.0.1 port=8007";
const size_t DB_CONNECTIONS_POOL_SIZE = 4;
drogon::orm::DbClientPtr client = nullptr;

const std::string INTEGRATIONS_TABLE = R"(
CREATE TABLE IF NOT EXISTS integrations (
  userId UUID PRIMARY KEY,
  twofa VARCHAR(10) DEFAULT NULL,
  discordId BIGINT DEFAULT NULL,
  telegramId BIGINT DEFAULT NULL
);
)";
const std::string REFRESH_TOKENS_TABLE = R"(
CREATE TABLE IF NOT EXISTS refresh_tokens (
  id SERIAL PRIMARY KEY,
  userId UUID NOT NULL,
  tokenHash BYTEA NOT NULL,
  expires_at TIMESTAMP NOT NULL
);
)";
const std::string TOKEN_HASH_INDEX = "CREATE UNIQUE INDEX IF NOT EXISTS idx_refresh_tokens_tokenhash ON refresh_tokens(tokenHash);";
const std::string TOKEN_EXPIRES_AT_INDEX = "CREATE INDEX IF NOT EXISTS idx_refresh_tokens_expires_at ON refresh_tokens(expires_at);";

void initDatabase() {
  client = drogon::orm::DbClient::newPgClient(DB_CONNECT_STRING, DB_CONNECTIONS_POOL_SIZE);
  client->execSqlSync(INTEGRATIONS_TABLE);
  client->execSqlSync(REFRESH_TOKENS_TABLE);
  client->execSqlSync(TOKEN_HASH_INDEX);
  client->execSqlSync(TOKEN_EXPIRES_AT_INDEX);
}
void closeDatabase() { client = nullptr; }

drogon::orm::DbClientPtr getDatabase() { return client; }
