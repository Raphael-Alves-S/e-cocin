#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/data/type/Type.hpp" 
#include "../services/ClientService.h"
#include "dto/ClientDto.h"
#include "dto/ClientOutDto.h"
#include <memory>

#include OATPP_CODEGEN_BEGIN(ApiController)

class ClientController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::ClientService> clientService;

public:
  ClientController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                   std::shared_ptr<ecocin::services::ClientService> service)
    : oatpp::web::server::api::ApiController(objectMapper),
      clientService(std::move(service)) {}

 ENDPOINT("POST", "/clients", createClient,
         BODY_DTO(Object<ClientDto>, body)) {
  Client client;
  if (body->name)  client.setName(std::string(body->name->c_str()));
  if (body->email) client.setEmail(std::string(body->email->c_str()));
  if (body->cpf)   client.setCpf(std::string(body->cpf->c_str()));

  auto msg = clientService->createClient(client);
  return createResponse(Status::CODE_200, oatpp::String(msg.c_str()));
}


  ENDPOINT("GET", "/clients", listAll) {
    auto vec = clientService->listAllClients();
    std::string msg = "clients=" + std::to_string(vec.size());
    return createResponse(Status::CODE_200, oatpp::String(msg.c_str()));
  }

ENDPOINT("GET", "/clients/{cpf}", getClient,
         PATH(String, cpf)) {
  const std::string key = cpf ? std::string(cpf->c_str()) : std::string{};
  auto c = clientService->getClientByCpf(key);
  if (!c) {
    return createResponse(Status::CODE_404, "Client not found");
  }

  // monta DTO de saída
  auto dto = ClientOutDto::createShared();
  dto->id    = c->getId();
  dto->name  = oatpp::String(c->getName().c_str());
  dto->email = oatpp::String(c->getEmail().c_str());
  dto->cpf   = oatpp::String(c->getCpf().c_str());

  // converte Timestamp -> epoch seconds (Int64)
  using namespace std::chrono;
  const auto secs = duration_cast<seconds>(c->getCreateDate().time_since_epoch()).count();
  dto->createDate = secs;

  // agora sim retorna JSON
  return createDtoResponse(Status::CODE_200, dto);
}
};

#include OATPP_CODEGEN_END(ApiController)
