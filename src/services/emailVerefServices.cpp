#include "services/emailVerefServices.hpp"
#include "db/Repository.hpp"
#include "drogon/utils/coroutine.h"
#include "http/rabbitMqClient.hpp"
#include "http/usersClient.hpp"
#include "services/hashUtil.hpp"
#include "services/minecraftTokenServices.hpp"
#include <drogon/utils/Utilities.h>
#include <fido.h>
#include <fido/bio.h>
#include <fido/credman.h>
#include <fido/err.h>
#include <fido/types.h>
#include <random>
#include <unordered_map>

namespace {
std::unordered_map<uint64_t, ConfirmationPandingEmailVereficationPending> data;

int genCode() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<int> dist(100000, 999999);

  return dist(gen);
}

} // namespace

drogon::Task<uint64_t> sendVereficationMail(ConfirmationPandingEmailVereficationPending dota2) {
  int code = genCode();
  uint64_t token = generateToken();
  std::chrono::steady_clock::time_point expiry = std::chrono::steady_clock::now() + std::chrono::minutes(15);

  if (dota2.passkey_challenge.has_value())
    if (!drogon::utils::secureRandomBytes(dota2.passkey_challenge->data(), dota2.passkey_challenge->size()))
      throw std::runtime_error("Ошибка генерации случайных байт");

  dota2.code = code;
  dota2.expiry = expiry;

  data.emplace(token, dota2);

  co_await sendEmailRequest(dota2.email, dota2.login, std::to_string(dota2.code));

  if (ConfirmationPandingEmailVereficationPending::Type::Login != dota2.type) {
    std::string hashedPassword;
    try {
      hashedPassword = hashPassword(dota2.password);
    } catch (const std::exception &e) {
      throw std::runtime_error("Password hashing failed: " + std::string(e.what()));
    }

    dota2.password = hashedPassword;
  }

  co_return token;
};

drogon::Task<std::optional<ConfirmationPandingEmailVereficationPending>> verifyEmail(uint64_t token, int code,
                                                                                     std::optional<RegistrationPasskey> passkey) {
  if (!data.contains(token)) {
    co_return std::nullopt;
  };

  auto now = std::chrono::steady_clock::now();

  if (now >= data[token].expiry) {
    data.erase(token);
    co_return std::nullopt;
  }

  if (code != data[token].code) {
    co_return std::nullopt;
  }

  UsersClient usersClient;

  switch (data[token].type) {
  case (ConfirmationPandingEmailVereficationPending::Type::Register): {
    auto dota1 = data[token];

    if (dota1.passkey_challenge.has_value()) {
      if (!passkey.has_value()) {
        throw std::runtime_error("Passkey not provided");
      }

      fido_cred_t *cred = fido_cred_new();
      if (!cred)
        throw std::runtime_error("Passkey check failed");

      int r =
          fido_cred_set_attstmt(cred, reinterpret_cast<const unsigned char *>(passkey->attestation.data()), passkey->attestation.size());
      if (r != FIDO_OK) {
        fido_cred_free(&cred);
        throw std::runtime_error(std::string("Ошибка парсинга attestation CBOR: ") + fido_strerr(r));
      }

      unsigned char client_data_hash[SHA256_DIGEST_LENGTH];
      SHA256(reinterpret_cast<const unsigned char *>(passkey->client_data.data()), passkey->client_data.size(), client_data_hash);

      fido_cred_set_clientdata_hash(cred, client_data_hash, sizeof(client_data_hash));

      fido_cred_set_rp(cred, "example.com", nullptr);

      r = fido_cred_verify(cred);
      if (r != FIDO_OK) {
        fido_cred_free(&cred);
        throw std::runtime_error(std::string("Ошибка верификации passkey: ") + fido_strerr(r));
      }

      const unsigned char *cred_id_ptr = fido_cred_id_ptr(cred);
      size_t cred_id_len = fido_cred_id_len(cred);

      // Получаем публичный ключ (ВНИМАНИЕ: libfido2 возвращает его сразу в формате OpenSSL EVP_PKEY!)
      // Тебе не нужно парсить COSE. Ты можешь сразу использовать его в OpenSSL.
      const unsigned char *pubkey_ptr = fido_cred_pubkey_ptr(cred);
      size_t pubkey_len = fido_cred_pubkey_len(cred);

      std::cout << "Успешная верификация!" << std::endl;
      std::cout << "Размер публичного ключа: " << pubkey_len << " байт." << std::endl;
      // Тут чета надо с ключами делать

      // Освобождаем память
      fido_cred_free(&cred);
    }

    // Создаём акк ебланчику чтобы не выткал
    auto createResult = co_await usersClient.createUser(dota1.login, dota1.password);
    if (std::holds_alternative<HttpError>(createResult)) {
      auto err = std::get<HttpError>(createResult);
      throw std::runtime_error("User creation failed: " + err.message);
    }

    UserResponseDto createdUser = std::get<UserResponseDto>(createResult);

    dota1.userId = UUID::fromString(createdUser.data.id);

    // Записываем ебланчику email чтобы не выткал х2
    bool emailSet = co_await Repository::IntegrationRepo::setEmail(dota1.userId.toString(), dota1.email);
    if (!emailSet) {
      throw std::runtime_error("failed to set email userID: " + dota1.userId.toString());
    }

    break;
  }
  case (ConfirmationPandingEmailVereficationPending::Type::Login): {
    auto satan = data[token];

    bool emailSet = co_await Repository::IntegrationRepo::setEmail(satan.userId.toString(), satan.email);
    if (!emailSet) {
      throw std::runtime_error("failed to set email userID: " + satan.userId.toString());
    }
    break;
  }
  }
  co_return data[token];
};

drogon::Task<void> resendEmail(uint64_t token) {
  if (!data.contains(token)) {
    co_return;
  };

  ConfirmationPandingEmailVereficationPending исус = data[token];

  int code = genCode();
  std::chrono::steady_clock::time_point expiry = std::chrono::steady_clock::now() + std::chrono::minutes(15);

  исус.code = code;
  исус.expiry = expiry;

  co_await sendEmailRequest(исус.email, исус.login, std::to_string(исус.code));
}
