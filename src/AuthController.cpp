#include "RequestCheck.hpp"
#include "ResponseHandler.hpp"
#include "codes.hpp"
#include "db/Repository.hpp"
#include "drogon/HttpResponse.h"
#include "drogon/HttpTypes.h"
#include "drogon/utils/coroutine.h"
#include "dto/userDto.hpp"
#include "dto/userResponseDto.hpp"
#include "http/usersClient.hpp"
#include "services/emailVerefServices.hpp"
#include "services/hashUtil.hpp"
#include "services/httpUtils.hpp"
#include "services/jwtUtil.hpp"
#include "services/minecraftTokenServices.hpp"
#include "services/uuidUtils.hpp"
#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <endian.h>
#include <json/config.h>
#include <json/value.h>
#include <stdexcept>
using namespace drogon;

class AuthController : public HttpController<AuthController> {
public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::registerEndpoint, "/auth/register", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::altRegisterEndpoint, "/auth/alt/register", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::loginEndpoint, "/auth/login", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::altLoginEndpoint, "/auth/alt/login", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::refreshEndpoint, "/auth/refresh", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::popGameTokenEndpoint, "/auth/pop_game_token", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::checkRefreshTokenEndpoint, "/auth/check_refresh_token", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::confirmEmailEndpoint, "/auth/confirm_email", Post, "TraceIdMiddleware", "LoggerMiddleware");
  ADD_METHOD_TO(AuthController::resentEmailEndpoint, "/auth/resend_email", Post, "TraceIdMiddleware", "LoggerMiddleware");
  METHOD_LIST_END

  Task<HttpResponsePtr> registerEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string login = RequestCheck::requireString(request, *json, "login");
      std::string password = RequestCheck::requireString(request, *json, "password");
      std::string email = RequestCheck::requireString(request, *json, "email");

      UsersClient usersClient;

      // Проверяем существования беззубого существа (базисный игрок элизиума)
      auto userCheckResult = co_await usersClient.getUserById(login); // TODO: сделать нормальнй метод
      if (!std::holds_alternative<HttpError>(userCheckResult)) {
        co_return ResponseHandler::error(request, "Login already exists", Codes::Error::USER_ALREADY_EXISTS);
      } else {
        auto err = std::get<HttpError>(userCheckResult);
        if (err.httpStatus != 404) {
          co_return ResponseHandler::error(request, "Error checking user existence: " + err.message, Codes::Error::USER_CREATION_FAILED);
        }
      }

      ConfirmationPandingEmailVereficationPending cpevp;

      cpevp.email = email;
      cpevp.login = login;
      cpevp.password = password;
      cpevp.type = ConfirmationPandingEmailVereficationPending::Type::Register;

      uint64_t svmRes = co_await sendVereficationMail(cpevp);
      uint8_t *bytes = reinterpret_cast<uint8_t *>(&svmRes);

      Json::Value res;
      res["email_verefication_token"] = utils::base64Encode(bytes, 8);

      co_return ResponseHandler::success(request, Codes::Success::REGISTRATION_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> altRegisterEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string login = RequestCheck::requireString(request, *json, "login");
      std::string password = RequestCheck::requireString(request, *json, "password");

      UsersClient usersClient;

      // Проверяем существования беззубого существа (базисный игрок элизиума)
      auto userCheckResult = co_await usersClient.getUserById(login); // TODO: сделать нормальнй метод
      if (!std::holds_alternative<HttpError>(userCheckResult)) {
        co_return ResponseHandler::error(request, "Login already exists", Codes::Error::USER_ALREADY_EXISTS);
      } else {
        auto err = std::get<HttpError>(userCheckResult);
        if (err.httpStatus != 404) {
          co_return ResponseHandler::error(request, "Error checking user existence: " + err.message, Codes::Error::USER_CREATION_FAILED);
        }
      }

      // Хешируем пароль
      std::string hashedPassword;
      try {
        hashedPassword = hashPassword(password);
      } catch (const std::exception &e) {
        co_return ResponseHandler::error(request, "Password hashing failed: " + std::string(e.what()), Codes::Error::HASHING_FAILED);
      }
      // Создаём ебланчика
      auto createResult = co_await usersClient.createUser(login, hashedPassword);
      if (std::holds_alternative<HttpError>(createResult)) {
        auto err = std::get<HttpError>(createResult);
        co_return ResponseHandler::error(request, "User creation failed: " + err.message, Codes::Error::USER_CREATION_FAILED);
      }

      UserResponseDto createdUser = std::get<UserResponseDto>(createResult);

      std::vector<uint8_t> refreshData(32);
      utils::secureRandomBytes(refreshData.data(), refreshData.size());
      auto refreshTokenHash = getRefreshTokenHash(refreshData);

      bool result = co_await Repository::RefreshTokenRepo::save(UUID::fromString(createdUser.data.id), refreshTokenHash, 30 * 24 * 60 * 60);
      if (!result) {
        throw std::runtime_error("Не удалось сохранить refresh token");
      }

      std::string refreshToken = utils::base64Encode(refreshData.data(), refreshData.size());

      Json::Value res;
      res["refresh_token"] = refreshToken;

      co_return ResponseHandler::success(request, Codes::Success::REGISTRATION_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> loginEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string login = RequestCheck::requireString(request, *json, "login");
      std::string password = RequestCheck::requireString(request, *json, "password");

      UsersClient usersClient;
      std::string clientIp = getClientIp(request);

      // Получаем информацию о игроке, включая его пароль
      auto userCheckResult = co_await usersClient.getUserById(login, true);
      if (std::holds_alternative<HttpError>(userCheckResult)) {
        auto err = std::get<HttpError>(userCheckResult);
        if (err.httpStatus == 404) {
          co_return ResponseHandler::error(request, "User not found", Codes::Error::USER_NOT_FOUND);
        }
        co_return ResponseHandler::error(request, "Error checking user existence: " + err.message, Codes::Error::AUTH_FAILED);
      }
      UserResponseDto user = std::get<UserResponseDto>(userCheckResult);

      if (!user.data.passwordHash) {
        throw std::logic_error("Password hash is null");
      }

      if (!verifyPassword(*user.data.passwordHash, password)) {
        LOG_INFO << "[AUTH][LOGIN_FAILED] login=" << login << " ip=" << clientIp;
        co_return ResponseHandler::error(request, "Password invalid", Codes::Error::PASSWORD_INVALID);
      }

      std::optional<Repository::Integration> userIntegrations = co_await Repository::IntegrationRepo::getByUserId(user.data.id);
      if (!userIntegrations || !userIntegrations->email) {
        std::string email = RequestCheck::requireString(request, *json, "email");

        ConfirmationPandingEmailVereficationPending cpevp;

        cpevp.userId = UUID::fromString(user.data.id);
        cpevp.email = email;
        cpevp.login = login;
        cpevp.type = ConfirmationPandingEmailVereficationPending::Type::Login;

        uint64_t svmRes = co_await sendVereficationMail(cpevp);
        uint8_t *bytes = reinterpret_cast<uint8_t *>(&svmRes);

        Json::Value res;
        res["email_verefication_token"] = utils::base64Encode(bytes, 8);

        co_return ResponseHandler::success(request, Codes::Error::EMAIL_NOT_SET, res);
      }

      // генерим refresh токен
      std::vector<uint8_t> refreshData(32);
      utils::secureRandomBytes(refreshData.data(), refreshData.size());
      auto refreshTokenHash = getRefreshTokenHash(refreshData);

      bool result = co_await Repository::RefreshTokenRepo::save(UUID::fromString(user.data.id), refreshTokenHash, 30 * 24 * 60 * 60);
      if (!result) {
        throw std::runtime_error("Не удалось сохранить refresh token");
      }

      // Делаем access_token
      AccessTokenData tokenData;
      tokenData.uuid = UUID::fromString(user.data.id);
      tokenData.tokenHash = refreshTokenHash;

      std::string accessToken = generateAccessToken(tokenData);
      std::string refreshToken = utils::base64Encode(refreshData.data(), refreshData.size());

      Json::Value res;
      res["access_token"] = accessToken;
      res["refresh_token"] = refreshToken;

      LOG_INFO << "[AUTH][LOGIN_SUCCESS] login=" << login << " userId=" << user.data.id << " ip=" << clientIp;
      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> altLoginEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string login = RequestCheck::requireString(request, *json, "login");
      std::string password = RequestCheck::requireString(request, *json, "password");

      UsersClient usersClient;
      std::string clientIp = getClientIp(request);

      // Получаем информацию о игроке, включая его пароль
      auto userCheckResult = co_await usersClient.getUserById(login, true);
      if (std::holds_alternative<HttpError>(userCheckResult)) {
        auto err = std::get<HttpError>(userCheckResult);
        if (err.httpStatus == 404) {
          co_return ResponseHandler::error(request, "User not found", Codes::Error::USER_NOT_FOUND);
        }
        co_return ResponseHandler::error(request, "Error checking user existence: " + err.message, Codes::Error::AUTH_FAILED);
      }

      UserResponseDto user = std::get<UserResponseDto>(userCheckResult);

      if (!user.data.passwordHash) {
        throw std::logic_error("Password hash is null");
      }

      if (!verifyPassword(*user.data.passwordHash, password)) {
        LOG_INFO << "[AUTH][LOGIN_FAILED] login=" << login << " ip=" << clientIp;
        co_return ResponseHandler::error(request, "Password invalid", Codes::Error::PASSWORD_INVALID);
      }

      std::vector<uint8_t> refreshData(32);
      utils::secureRandomBytes(refreshData.data(), refreshData.size());
      auto refreshTokenHash = getRefreshTokenHash(refreshData);

      bool result = co_await Repository::RefreshTokenRepo::save(UUID::fromString(user.data.id), refreshTokenHash, 30 * 24 * 60 * 60);
      if (!result) {
        throw std::runtime_error("Не удалось сохранить refresh token");
      }

      LOG_INFO << "[AUTH][LOGIN_SUCCESS] login=" << login << " userId=" << user.data.id << " ip=" << clientIp;

      std::string refreshToken = utils::base64Encode(refreshData.data(), refreshData.size());

      Json::Value res;
      res["refresh_token"] = refreshToken;
      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    }
  }

  Task<HttpResponsePtr> confirmEmailEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);
      std::string emailVereficationToken = RequestCheck::requireString(request, *json, "email_verefication_token");
      std::string code = RequestCheck::requireString(request, *json, "code");

      auto data = utils::base64DecodeToVector(emailVereficationToken);
      if (data.size() != sizeof(uint64_t))
        co_return ResponseHandler::error(request, "Token invalid format", Codes::Error::INVALID_DATA);

      uint64_t token = 0;
      std::memcpy(&token, data.data(), sizeof(uint64_t));

      auto res = co_await verifyEmail(token, std::stoi(code));
      if (!res) {
        co_return ResponseHandler::error(request, Codes::Error::AUTH_FAILED);
      }

      if (res->type == ConfirmationPandingEmailVereficationPending::Type::Login ||
          res->type == ConfirmationPandingEmailVereficationPending::Type::Register) {

        std::vector<uint8_t> refreshData(32);
        utils::secureRandomBytes(refreshData.data(), refreshData.size());
        auto refreshTokenHash = getRefreshTokenHash(refreshData);

        bool result =
            co_await Repository::RefreshTokenRepo::save(UUID::fromString(res->userId.toString()), refreshTokenHash, 30 * 24 * 60 * 60);
        if (!result) {
          throw std::runtime_error("Не удалось сохранить refresh token");
        }

        AccessTokenData tokenData;
        tokenData.uuid = res->userId;
        tokenData.tokenHash = refreshTokenHash;

        std::string accessToken = generateAccessToken(tokenData);
        std::string refreshToken = utils::base64Encode(refreshData.data(), refreshData.size());

        Json::Value res1;
        res1["access_token"] = accessToken;
        res1["refresh_token"] = refreshToken;

        LOG_INFO << "[AUTH][LOGIN_SUCCESS] login=" << res->login << " userId=" << res->userId.toString();
        co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res1);
      }

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> resentEmailEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);
      std::string emailVereficationToken = RequestCheck::requireString(request, *json, "email_verefication_token");

      auto data = utils::base64DecodeToVector(emailVereficationToken);
      if (data.size() != sizeof(uint64_t))
        co_return ResponseHandler::error(request, "Token invalid format", Codes::Error::INVALID_DATA);

      uint64_t token = 0;
      std::memcpy(&token, data.data(), sizeof(uint64_t));

      co_await resendEmail(token);

      co_return ResponseHandler::success(request, Codes::Success::RESEND_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  };

  Task<HttpResponsePtr> refreshEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string method = RequestCheck::requireString(request, *json, "method");
      RequestCheck::requireOneOf(request, "method", method, {"Web", "Game"});

      Repository::RefreshToken refreshToken = co_await RequestCheck::requireRefreshToken(request, *json, "refresh_token");

      UUID uuid = refreshToken.userId;

      if (method == "Game") {
        UsersClient usersClient;

        auto userCheckResult = co_await usersClient.getUserById(refreshToken.userId.toString());
        if (std::holds_alternative<HttpError>(userCheckResult)) {
          auto err = std::get<HttpError>(userCheckResult);
          if (err.httpStatus == 404) {
            co_return ResponseHandler::error(request, "User not found", Codes::Error::USER_NOT_FOUND);
          }
          co_return ResponseHandler::error(request, "Error checking user existence: " + err.message, Codes::Error::AUTH_FAILED);
        }
        UserResponseDto user = std::get<UserResponseDto>(userCheckResult);

        uint64_t token = createTokenForUser(uuid, user.data.name);
        uint8_t *bytes = reinterpret_cast<uint8_t *>(&token);

        Json::Value res;
        res["token"] = utils::base64Encode(bytes, 8);

        co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
      } else if (method == "Web") {
        AccessTokenData tokenData;
        tokenData.uuid = uuid;
        tokenData.tokenHash = refreshToken.tokenHash;

        std::string accessToken = generateAccessToken(tokenData);

        Json::Value res;
        res["token"] = accessToken;

        co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
      } else {
        throw std::runtime_error("Unexpected method " + method);
      }
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> popGameTokenEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);

      std::string gameToken = RequestCheck::requireString(request, *json, "game_token");

      auto data = utils::base64DecodeToVector(gameToken);
      if (data.size() != sizeof(uint64_t))
        co_return ResponseHandler::error(request, "Token invalid format", Codes::Error::INVALID_DATA);

      uint64_t token = 0;
      std::memcpy(&token, data.data(), sizeof(uint64_t));

      std::optional<Token> result = popToken(token);
      if (!result.has_value()) {
        co_return ResponseHandler::error(request, Codes::Error::GAME_TOKEN_INVALID);
      }

      Json::Value res;
      res["uuid"] = result->userUUID.toString();
      res["username"] = result->username;

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, res);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }

  Task<HttpResponsePtr> checkRefreshTokenEndpoint(HttpRequestPtr request) {
    try {
      const Json::Value *json = RequestCheck::requireJson(request);
      Repository::RefreshToken refreshToken = co_await RequestCheck::requireRefreshToken(request, *json, "refresh_token");

      co_return ResponseHandler::success(request, Codes::Success::AUTH_SUCCESS, Json::nullValue);
    } catch (const RequestCheck::ValidationError &error) {
      co_return error.response;
    } catch (const std::exception &ex) {
      co_return ResponseHandler::error(request, "Unexpected error: " + std::string(ex.what()), Codes::Error::USER_CREATION_FAILED);
    }
  }
};
