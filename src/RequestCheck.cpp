#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "codes.hpp"

namespace RequestCheck {

const Json::Value *requireJson(const drogon::HttpRequestPtr &request) {
  auto json = request->jsonObject();
  if (!json)
    throw ValidationError(ResponseHandler::error(request, Codes::Error::JSON_INVALID));

  return json.get();
}

std::string requireString(const drogon::HttpRequestPtr &request, const Json::Value &json, const std::string &field) {
  const auto &v = json.get(field, Json::nullValue);
  if (v.isNull() || !v.isString())
    throw ValidationError(ResponseHandler::error(request, field + " not defined or it is not string", Codes::Error::INVALID_DATA));

  return v.asString();
}

#include "ResponseHandler.hpp"
#include "codes.hpp"
void requireOneOf(const drogon::HttpRequestPtr &request, const std::string &field, const std::string &value,
                  const std::initializer_list<std::string> &allowed) {
  if (std::find(allowed.begin(), allowed.end(), value) == allowed.end())
    throw ValidationError(ResponseHandler::error(request, field + " has incorrect value", Codes::Error::METHOD_INCORRECT));
}

} // namespace RequestCheck
