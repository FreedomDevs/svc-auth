#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
namespace services {
class RateLimiter {
public:
  bool allow(const std::string &key, int limit, std::chrono::seconds window);

private:
  std::unordered_map<std::string, std::vector<std::chrono::steady_clock::time_point>> storage;
};
} // namespace services
