#pragma once
#include <string>

enum class HttpErrorType { Network, HttpStatus, Serialization, Deserialization };

struct HttpError {
  HttpErrorType type;
  int httpStatus{0};
  std::string message;
};
