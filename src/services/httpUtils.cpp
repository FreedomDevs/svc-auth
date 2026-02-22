#include "services/httpUtils.hpp"

std::string getClientIp(drogon::HttpRequestPtr request) {
  auto xff = request->getHeader("X-Forwarded-For");
  if (!xff.empty()) {
    return xff.substr(0, xff.find(','));
  }
  return request->peerAddr().toIp();
}
