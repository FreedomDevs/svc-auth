#pragma once
#include "config.hpp"
#include "dto/ipDoxResponseDto.hpp"
#include "http/httpResult.hpp"
#include "httpClient.hpp"
#include <ranges>

class IpDoxClient {
public:
  IpDoxClient() : http_(config::DOX_SERVICE_URL) {}

  drogon::Task<HttpResult<IpDoxResponseDto>> getUserById(const std::vector<std::string> &ips, const std::optional<std::string> &lang) {
    auto ips_str = ips | std::views::join_with(',');

    return http_.get<IpDoxResponseDto>(
        std::format("/dox/ip?ip={}{}", std::ranges::to<std::string>(ips_str), lang ? std::format("&lang={}", *lang) : ""));
  }

private:
  HttpClient http_;
};
