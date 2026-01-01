#pragma once
#include <array>
#include <cstdint>
#include <string>

class UUID {
private:
  std::array<uint8_t, 16> bytes;

public:
  UUID() {}
  explicit UUID(const std::array<uint8_t, 16> &data); // Инициализация байтами
  static UUID fromString(const std::string &str);     // Создать из строки
  std::string toString() const;                       // Преобразовать в строку

  const std::array<uint8_t, 16> &getBytes() const { return bytes; }

  bool operator==(const UUID &other) const;
  bool operator!=(const UUID &other) const;

  std::size_t hash() const;
};

namespace std {
template <> struct hash<UUID> {
  inline std::size_t operator()(const UUID &uuid) const noexcept { return uuid.hash(); }
};
} // namespace std
