#pragma once
#include "config.hpp"
#include <drogon/drogon.h>

drogon::Task<void> sendEmailRequest(std::string email, std::string name, std::string code) {
  auto client = drogon::HttpClient::newHttpClient(config::RABBIT_MQ_URL);

  Json::Value payload;
  payload["pattern"] = "send_email";

  Json::Value data;
  data["to"] = email;
  data["subject"] = "ElysiumID — Подтверждение регистрации";
  data["template"] = "confirm-account";

  Json::Value context;
  context["name"] = name;
  context["code"] = code;

  data["context"] = context;
  payload["data"] = data;

  Json::StreamWriterBuilder writer;
  std::string message = Json::writeString(writer, payload);

  Json::Value body;
  body["properties"] = Json::objectValue;
  body["routing_key"] = config::RABBIT_MQ_QUEUE_NAME;
  body["payload"] = message;
  body["payload_encoding"] = "string";

  auto req = drogon::HttpRequest::newHttpJsonRequest(body);

  req->setMethod(drogon::Post);
  req->setPath("/api/exchanges/%2F/amq.default/publish");

  req->addHeader("Authorization", "Basic " + drogon::utils::base64Encode(config::RABBIT_MQ_LOGIN + ":" + config::RABBIT_MQ_PASSWORD));

  auto res = co_await client->sendRequestCoro(req);
  if (res->statusCode() == 200) {
    LOG_INFO << res->getBody();
  } else {
    LOG_ERROR << "Publish failed";
    throw std::runtime_error("Publish failed");
  }

  co_return;
}
