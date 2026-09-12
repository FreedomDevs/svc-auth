#include "services/uuidUtils.hpp"
#include <chrono>
#include <optional>

struct TwoFactorVerefPending {
  UUID uuid;
  std::chrono::steady_clock::time_point expToken;
};

int generate2FaCode(UUID uuid);

std::optional<UUID> getUuidWhithCode(int code);
