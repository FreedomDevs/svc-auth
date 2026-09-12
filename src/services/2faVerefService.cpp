#include <random>
#include <services/2faVerefService.hpp>
#include <unordered_map>

namespace {
std::unordered_map<uint64_t, TwoFactorVerefPending> data;

int genCode() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<int> dist(100000, 999999);

  return dist(gen);
}
} // namespace

int generate2FaCode(UUID uuid) {
  int code = genCode();
  std::chrono::steady_clock::time_point expiry = std::chrono::steady_clock::now() + std::chrono::minutes(15);

  TwoFactorVerefPending data2;
  data2.expToken = expiry;
  data2.uuid = uuid;

  data.emplace(code, data2);

  return code;
}

std::optional<UUID> getUuidWhithCode(int code) {
  if (!data.contains(code)) {
    return std::nullopt;
  }

  auto now = std::chrono::steady_clock::now();

  if (now >= data[code].expToken) {
    data.erase(code);
    return std::nullopt;
  }

  UUID uuiddd = data[code].uuid;
  data.erase(code);

  return uuiddd;
}
