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

// O ClientController gerencia as requisições HTTP para a entidade Cliente.
// Seguindo o padrão MVC, ele atua como a camada de entrada da aplicação,
// recebendo requisições, delegando a lógica de negócio para a camada de serviço
// e formatando as respostas para o consumidor da API.
class ClientController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::ClientService> clientService;

// Converte um objeto de domínio 'Client' em um 'ClientOutDto'.
// Este método privado é um exemplo de encapsulamento e do padrão DTO (Data Transfer Object).
// Ele garante que a representação interna do cliente seja desacoplada da estrutura de dados
// exposta pela API, permitindo modificações internas sem impactar os clientes da API.
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
  // O construtor utiliza injeção de dependência para obter o serviço de cliente.
  // Este design, alinhado com os princípios SOLID, promove o baixo acoplamento,
  // tornando o controller mais fácil de testar e manter, pois sua dependência
  // de serviço pode ser facilmente substituída por um "mock" em testes.
  ClientController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                   std::shared_ptr<ecocin::services::ClientService> service)
    : oatpp::web::server::api::ApiController(objectMapper),
      clientService(std::move(service)) {}

 // Endpoint para a criação de um novo cliente.
 // Ele recebe os dados do cliente no corpo da requisição (BODY_DTO),
 // os converte para um objeto de domínio e invoca o serviço para processar a criação.
 // A responsabilidade do controller é orquestrar esse fluxo e retornar uma resposta HTTP adequada.
 ENDPOINT("POST", "/clients", createClient,
         BODY_DTO(Object<ClientDto>, body)) {
  Client client;
  if (body->name)  client.setName(std::string(body->name->c_str()));
  if (body->email) client.setEmail(std::string(body->email->c_str()));
  if (body->cpf)   client.setCpf(std::string(body->cpf->c_str()));

  auto msg = clientService->createClient(client);
  return createResponse(Status::CODE_200, oatpp::String(msg.c_str()));
}


  // Endpoint para listar todos os clientes cadastrados.
  // O controller chama o serviço para obter a lista de clientes,
  // converte cada objeto de domínio para seu DTO correspondente e retorna
  // a lista como uma resposta JSON, demonstrando a separação de responsabilidades.
  ENDPOINT("GET", "/clients", listAll) {
    const auto clients = clientService->listAllClients();
    
    auto arr = oatpp::List<oatpp::Object<ClientOutDto>>::createShared();
    for (const auto& c : clients) {
      arr->push_back(toOutDto(c));
    }
    return createDtoResponse(Status::CODE_200, arr);
  }

// Endpoint para buscar um cliente específico pelo seu CPF.
// O CPF é extraído do caminho da URL (PATH). O controller então usa o serviço
// para encontrar o cliente. Se o cliente não for encontrado, ele retorna um status 404 (Not Found),
// tratando adequadamente os diferentes resultados da lógica de negócio.
ENDPOINT("GET", "/clients/cpf/{cpf}", getClient,
         PATH(String, cpf)) {
  const std::string key = cpf ? std::string(cpf->c_str()) : std::string{};
  auto client = clientService->getClientByCpf(key);
  if (!client) {
    return createResponse(Status::CODE_404, "Client not found");
  }
  return createDtoResponse(Status::CODE_200, toOutDto(*client));
}

  // Endpoint para buscar um cliente pelo seu ID técnico.
  // Similar à busca por CPF, este método lida com a extração do parâmetro da URL,
  // a chamada ao serviço e a formatação da resposta, seja ela de sucesso (200 OK com os dados)
  // ou de erro (404 Not Found).
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
