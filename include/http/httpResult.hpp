#pragma once
#include "httpError.hpp"
#include <variant>

template <typename T> using HttpResult = std::variant<T, HttpError>;
