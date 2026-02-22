#pragma once
#include <drogon/HttpController.h>
#include <string>

std::string getClientIp(drogon::HttpRequestPtr request);
