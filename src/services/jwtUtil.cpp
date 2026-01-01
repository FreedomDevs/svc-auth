#include "services/jwtUtil.hpp"
#include <chrono>
#include <drogon/utils/Utilities.h>

using namespace drogon;

namespace {

// Конвертируем tokenHash в Base64
std::string hashToBase64(const std::array<uint8_t, 32> &hash) { return utils::base64Encode(hash.data(), hash.size()); }

// Конвертируем Base64 обратно в tokenHash
std::array<uint8_t, 32> base64ToHash(const std::string &b64) {
  std::string decoded = utils::base64Decode(b64);
  if (decoded.size() != 32)
    throw std::runtime_error("Invalid token hash size");
  std::array<uint8_t, 32> arr;
  std::copy(decoded.begin(), decoded.end(), arr.begin());
  return arr;
}

} // namespace

std::string generateAccessToken(const AccessTokenData &data) {
  auto now = std::chrono::system_clock::now();
  auto iat = now;
  auto exp = now + std::chrono::seconds(ttlSeconds);

  auto token = jwt::create()
                   .set_issuer("svc-auth")
                   .set_type("JWT")
                   .set_issued_at(iat)
                   .set_expires_at(exp)
                   .set_payload_claim("uuid", jwt::claim(data.uuid.toString()))
                   .set_payload_claim("tokenHash", jwt::claim(hashToBase64(data.tokenHash)))
                   .sign(jwt::algorithm::hs256{secret});

  return token;
}

std::optional<AccessTokenData> verifyAccessToken(const std::string &token) {
  try {
    auto decoded = jwt::decode(token);

    // Проверка подписи
    jwt::verify().allow_algorithm(jwt::algorithm::hs256{secret}).with_issuer("svc-auth").verify(decoded);

    // Проверка истечения
    auto now = std::chrono::system_clock::now();
    if (decoded.has_expires_at() && decoded.get_expires_at() < now) {
      return std::nullopt;
    }

    AccessTokenData data;
    data.uuid = UUID::fromString(decoded.get_payload_claim("uuid").as_string());
    data.tokenHash = base64ToHash(decoded.get_payload_claim("tokenHash").as_string());

    return data;
  } catch (const std::exception &) {
    return std::nullopt;
  }
}
