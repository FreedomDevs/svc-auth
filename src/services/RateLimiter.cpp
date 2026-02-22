#include "services/RateLimiter.hpp"

bool services::RateLimiter::allow(const std::string &key, int limit, std::chrono::seconds window) {
  auto now = std::chrono::steady_clock::now();
  auto &entry = storage[key];

  entry.erase(std::remove_if(entry.begin(), entry.end(), [&](auto t) { return now - t > window; }), entry.end());

  if (entry.size() >= static_cast<size_t>(limit)) {
    return false;
  }

  entry.push_back(now);
  return true;
}
