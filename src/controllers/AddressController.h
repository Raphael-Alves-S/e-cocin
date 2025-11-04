#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/data/type/Type.hpp"
#include "../services/AddressService.h"
#include "dto/AddressDto.h"
#include "dto/AddressOutDto.h"
#include <memory>
#include <chrono>

#include OATPP_CODEGEN_BEGIN(ApiController)

// O AddressController é a porta de entrada para as requisições HTTP relacionadas a endereços.
// Ele herda de ApiController do framework OATPP, que gerencia o roteamento e a serialização/desserialização de dados.
// Esta classe demonstra o padrão de projeto MVC (Model-View-Controller), onde o Controller
// recebe a entrada do usuário (requisição HTTP), interage com o Model (através da camada de Serviço)
// e retorna uma View (a resposta HTTP, geralmente em JSON).
class AddressController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::AddressService> addressService;

  // Converte um objeto de domínio 'Address' para um 'AddressOutDto' (Data Transfer Object).
  // O uso de DTOs é uma prática de encapsulamento que desacopla a representação interna
  // do domínio daquela exposta pela API. Isso permite que o modelo de domínio evolua
  // sem quebrar os contratos da API.
  static oatpp::Object<AddressOutDto> toOutDto(const Address& a) {
    using namespace std::chrono;
    auto dto = AddressOutDto::createShared();
    dto->id          = a.getId();
    dto->clientId    = a.getClientId();
    dto->street      = oatpp::String(a.getStreet().c_str());
    dto->number      = oatpp::String(a.getNumber().c_str());
    dto->city        = oatpp::String(a.getCity().c_str());
    dto->state       = oatpp::String(a.getState().c_str());
    dto->zip         = oatpp::String(a.getZip().c_str());
    dto->addressType = oatpp::String(a.getAddressType().c_str());
    dto->createDate  = duration_cast<seconds>(a.getCreateDate().time_since_epoch()).count();
    return dto;
  }

public:
  // O construtor utiliza injeção de dependência para receber o serviço de endereço.
  // Isso segue o princípio da Inversão de Dependência, tornando o controller
  // independente da forma como o serviço é criado e facilitando os testes unitários
  // ao permitir a injeção de um serviço "mock".
  AddressController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                    std::shared_ptr<ecocin::services::AddressService> service)
  : oatpp::web::server::api::ApiController(objectMapper),
    addressService(std::move(service)) {}

  // Define o endpoint para criar um novo endereço.
  // A responsabilidade deste método é puramente de controle: ele extrai os dados da
  // requisição (BODY_DTO), faz uma validação de entrada básica (CPF obrigatório),
  // chama a camada de serviço para executar a lógica de negócio e, por fim,
  // formata a resposta HTTP apropriada (201 Created ou 400 Bad Request).
  ENDPOINT("POST", "/addresses", createAddress,
           BODY_DTO(Object<AddressDto>, body)) {
    if (!body || !body->cpf || body->cpf->empty()) {
      return createResponse(Status::CODE_400, "cpf é obrigatório");
    }

    Address in;
    if (body->street)      in.setStreet(std::string(body->street->c_str()));
    if (body->number)      in.setNumber(std::string(body->number->c_str()));
    if (body->city)        in.setCity(std::string(body->city->c_str()));
    if (body->state)       in.setState(std::string(body->state->c_str()));
    if (body->zip)         in.setZip(std::string(body->zip->c_str()));
    if (body->addressType) in.setAddressType(std::string(body->addressType->c_str()));


    auto created = addressService->create(std::string(body->cpf->c_str()), in);
    if (!created) {
      return createResponse(Status::CODE_400, "CPF não encontrado");
    }
    return createResponse(Status::CODE_201);
  }

  // Define o endpoint para listar todos os endereços.
  // O controller delega a busca dos dados para a camada de serviço, recebe a lista
  // de objetos de domínio e a transforma em uma lista de DTOs para a resposta.
  // Essa transformação garante que a API exponha apenas os dados necessários e no formato correto.
  ENDPOINT("GET", "/addresses", listAll) {
    const auto items = addressService->listAll();
    auto arr = oatpp::List<oatpp::Object<AddressOutDto>>::createShared();
    for (const auto& addr : items) {
      arr->push_back(toOutDto(addr));
    }
    return createDtoResponse(Status::CODE_200, arr);
  }

  // Define o endpoint para listar os endereços de um cliente específico.
  // Este método extrai o CPF da URL (PATH), passa-o para o serviço e formata
  // a lista resultante em uma resposta JSON. A responsabilidade do controller
  // é gerenciar o fluxo da requisição e resposta, mantendo a lógica de negócio
  // isolada na camada de serviço.
  ENDPOINT("GET", "/clients/{cpf}/addresses", listAddressesByCpf,
           PATH(String, cpf)) {
    auto list = addressService->listByCpf(std::string(cpf->c_str()));
    auto arr = oatpp::List<oatpp::Object<AddressOutDto>>::createShared();
    for (const auto& a : list) arr->push_back(toOutDto(a));
    return createDtoResponse(Status::CODE_200, arr);
  }
};

#include OATPP_CODEGEN_END(ApiController)
