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

static oatpp::Object<ClientOutDto> toOutDto(const Client& c) {
    auto dto = ClientOutDto::createShared();
    dto->id    = c.getId();
    dto->name  = oatpp::String(c.getName().c_str());
    dto->email = oatpp::String(c.getEmail().c_str());
    dto->cpf   = oatpp::String(c.getCpf().c_str());

    using namespace std::chrono;
    dto->createDate = duration_cast<seconds>(c.getCreateDate().time_since_epoch()).count();
    return dto;
  }

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
    const auto clients = clientService->listAllClients();
    
    auto arr = oatpp::List<oatpp::Object<ClientOutDto>>::createShared();
    for (const auto& c : clients) {
      arr->push_back(toOutDto(c));
    }
    return createDtoResponse(Status::CODE_200, arr);
  }

ENDPOINT("GET", "/clients/cpf/{cpf}", getClient,
         PATH(String, cpf)) {
  const std::string key = cpf ? std::string(cpf->c_str()) : std::string{};
  auto client = clientService->getClientByCpf(key);
  if (!client) {
    return createResponse(Status::CODE_404, "Client not found");
  }
  return createDtoResponse(Status::CODE_200, toOutDto(*client));
}

  ENDPOINT("GET", "/clients/{id}", getClientById,
           PATH(Int64, id)) {
    auto client = clientService->getClientById(id);
    if (!client) {
      return createResponse(Status::CODE_404, "Client not found");
    }
  return createDtoResponse(Status::CODE_200, toOutDto(*client));
}

};

#include OATPP_CODEGEN_END(ApiController)
