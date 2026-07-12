#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "drogon/HttpController.h"

using namespace drogon;

class ClientInfoController : public HttpController<ClientInfoController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(ClientInfoController::getClientInfo, "/auth/client_info/{client_id}", Get, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(ClientInfoController::putClientInfo, "/auth/client_info", Put, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(ClientInfoController::deleteClientInfo, "/auth/client_info/{client_id}", Delete, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(ClientInfoController::listClientInfo, "/auth/client_info", Get, "TraceIdMiddleware", "LoggerMiddleware");
  METHOD_LIST_END

  Task<HttpResponsePtr> putClientInfo(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      Repository::ClientInfo client_info;
      client_info.id = RequestCheck::requireString(request, *json, "id");
      client_info.name = RequestCheck::requireString(request, *json, "name");
      client_info.description = RequestCheck::requireString(request, *json, "description");
      client_info.redirect_url = RequestCheck::requireString(request, *json, "redirect_url");

      bool status = co_await Repository::ClientInfoRepo::put(client_info);

      if (!status) {
        co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
      }
      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> deleteClientInfo(HttpRequestPtr request, std::string client_id) {
    try {
      auto [status, found] = co_await Repository::ClientInfoRepo::delet(client_id);

      if (!status) {
        co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
      }

      if (!found)
        co_return ResponseHandler::error(request, Codes::Error::NOT_FOUND);

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> getClientInfo(HttpRequestPtr request, std::string client_id) {
    try {
      auto [status, data] = co_await Repository::ClientInfoRepo::get(client_id);

      if (!status) {
        co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
      }

      if (!data.has_value()) {
        co_return ResponseHandler::error(request, Codes::Error::NOT_FOUND);
      }
      Json::Value res;
      res["client_name"] = data->name;
      res["description"] = data->description;
      res["redirect_url"] = data->redirect_url;

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> listClientInfo(HttpRequestPtr request) {
    try {
      auto [status, data] = co_await Repository::ClientInfoRepo::list();

      if (!status) {
        co_return ResponseHandler::error(request, Codes::Error::INTERNAL_ERROR);
      }

      Json::Value res;

      for (auto i = data.cbegin(); i < data.cend(); i++) {
        Json::Value item;
        item["id"] = i->id;
        item["name"] = i->description;
        item["description"] = i->description;
        item["redirect_url"] = i->redirect_url;

        res.append(item);
      }

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }
};
