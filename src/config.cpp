#include "config.hpp"
#include <algorithm>
#include <sstream>
#include <trantor/utils/Logger.h>

namespace config {

std::string getEnvOrDefault(const std::string &key, const std::string &defaultValue) {
  const char *value = std::getenv(key.c_str());
  if (value == nullptr || std::string(value).empty()) {
    return defaultValue;
  }
  return std::string(value);
}

bool parseBoolSafe(const std::string &s) {
  std::string lowerStr = s;
  std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
  if (lowerStr == "true" || lowerStr == "1") {
    return true;
  }
  if (lowerStr == "false" || lowerStr == "0") {
    return false;
  }
  return false; // если не удалось распарсить
}

int getEnvIntOrDefault(const std::string &key, int defaultValue) {
  const char *valueC = std::getenv(key.c_str());
  if (valueC == nullptr || std::string(valueC).empty()) {
    return defaultValue;
  }

  std::string valueStr(valueC);
  int v;
  std::istringstream iss(valueStr);
  if (!(iss >> v)) {
    LOG_WARN << "WARN: не удалось преобразовать " << key << "='" << valueStr << "' в int, используем default=" << defaultValue;
    return defaultValue;
  }

  return v;
}

} // namespace config
