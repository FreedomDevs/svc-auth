#include "services/uuidUtils.hpp"
#include <iomanip>
#include <sstream>

// Инициализация конкретными байтами
UUID::UUID(const std::array<uint8_t, 16> &data) : bytes(data) {}

// Создание из строки вида xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
UUID UUID::fromString(const std::string &str) {
  std::array<uint8_t, 16> data{};
  int j = 0;
  for (size_t i = 0; i < str.size() && j < 16; ++i) {
    if (str[i] == '-')
      continue;
    std::string byteStr = str.substr(i, 2);
    data[j++] = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
    ++i;
  }
  return UUID(data);
}

// Преобразование в строку
std::string UUID::toString() const {
  std::ostringstream oss;
  for (size_t i = 0; i < bytes.size(); ++i) {
    if (i == 4 || i == 6 || i == 8 || i == 10)
      oss << '-';
    oss << std::hex << std::setw(2) << std::setfill('0') << int(bytes[i]);
  }
  return oss.str();
}

std::size_t UUID::hash() const {
  std::size_t firstHalf = 0;
  std::size_t secondHalf = 0;
  for (int i = 0; i < 8; ++i)
    firstHalf = (firstHalf << 8) | bytes[i];
  for (int i = 8; i < 16; ++i)
    secondHalf = (secondHalf << 8) | bytes[i];
  return firstHalf ^ secondHalf; // XOR половин
}

// Операторы сравнения
bool UUID::operator==(const UUID &other) const { return bytes == other.bytes; }

bool UUID::operator!=(const UUID &other) const { return !(*this == other); }
