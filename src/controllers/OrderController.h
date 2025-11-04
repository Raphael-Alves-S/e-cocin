#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/data/type/Type.hpp"

#include "../services/OrderService.h"
#include "dto/OrderDto.h"
#include "dto/OrderOutDto.h"
#include "dto/AddressBriefDto.h"

#include <chrono>
#include <memory>

#include OATPP_CODEGEN_BEGIN(ApiController)

// O OrderController é responsável por gerenciar as requisições HTTP relacionadas a pedidos.
// Ele atua como o ponto de entrada para a criação e listagem de pedidos, orquestrando
// a interação entre a camada de apresentação (HTTP) e a camada de lógica de negócio (serviço).
// Este controller demonstra claramente a aplicação do padrão MVC.
class OrderController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::OrderService> orderService_;

  // Converte um objeto de domínio 'Address' em um 'AddressBriefDto'.
  // Este DTO mais enxuto é usado para encapsular as informações do endereço de entrega
  // dentro do DTO principal do pedido, evitando a exposição de dados desnecessários.
  static oatpp::Object<AddressBriefDto> toAddressBriefDto(const Address& a) {
    auto dto = AddressBriefDto::createShared();
    dto->street      = a.getStreet().c_str();
    dto->number      = a.getNumber().c_str();
    dto->city        = a.getCity().c_str();
    dto->state       = a.getState().c_str();
    dto->zip         = a.getZip().c_str();
    dto->addressType = a.getAddressType().c_str();
    return dto;
  }

  // Converte a estrutura 'OrderDetails' (que agrega dados de Pedido, Cliente, Produto e Endereço)
  // em um 'OrderOutDto'. O uso de um DTO de saída específico para pedidos (`OrderOutDto`)
  // é um exemplo de encapsulamento e abstração, pois permite que a API exponha uma visão
  // consolidada e rica dos dados, sem acoplar o cliente da API à estrutura interna do domínio.
  static oatpp::Object<OrderOutDto> toOutDto(const ecocin::services::OrderDetails& d) {
    using namespace std::chrono;
    auto dto = OrderOutDto::createShared();
    dto->id                 = d.order.getId();
    dto->clientName         = d.client.getName().c_str();
    dto->productDescription = d.product.getDescription().c_str();
    dto->shippingAddress    = toAddressBriefDto(d.address);
    dto->quantity           = d.order.getQuantity();
    dto->unitPrice          = d.order.getUnitPrice(); // já veio do produto no create
    dto->totalPrice         = d.order.getTotalPrice();
    dto->status             = d.order.getStatus().c_str();
    dto->createDate         = duration_cast<seconds>(d.order.getCreateDate().time_since_epoch()).count();
    return dto;
  }

public:
  // O construtor utiliza injeção de dependência para receber o serviço de pedidos.
  // Esta abordagem, central para o princípio de Inversão de Dependência, torna o controller
  // mais modular, testável e fácil de manter, pois ele não depende de uma instância concreta do serviço.
  OrderController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                  std::shared_ptr<ecocin::services::OrderService> service)
    : oatpp::web::server::api::ApiController(objectMapper),
      orderService_(std::move(service)) {}

  // Endpoint para criar um novo pedido.
  // A responsabilidade do controller é validar a presença dos dados essenciais na requisição,
  // extrair esses dados e passá-los para a camada de serviço, que contém a lógica de negócio
  // complexa. O controller então traduz o resultado do serviço em uma resposta HTTP apropriada.
  ENDPOINT("POST", "/orders", createOrder,
           BODY_DTO(Object<OrderDto>, body)) {
    if (!body || !body->cpf || !body->sku || !body->shippingAddressType) {
      return createResponse(Status::CODE_400, "cpf, sku e shippingAddressType são obrigatórios");
    }
    const int qty = body->quantity ? (int)*body->quantity : 1;

    auto created = orderService_->createByCpfSkuAndType(
      body->cpf->c_str(),
      body->sku->c_str(),
      body->shippingAddressType->c_str(),
      qty
    );

    if (!created) {
      return createResponse(Status::CODE_400,
        "Cliente/produto não encontrado ou endereço inválido para o tipo informado");
    }
    // retorno simples do recurso criado (sem enriquecer — GET abaixo enriquece)
    // Se preferir já enriquecer, faça um getById + composição como em listagem.
    return createResponse(Status::CODE_201);
  }

  // Endpoint para listar os pedidos de um cliente, identificado pelo CPF via query string.
  // O controller extrai o parâmetro da query, invoca o serviço para obter os detalhes
  // dos pedidos e, em seguida, utiliza os métodos de conversão para DTO para formatar
  // a resposta. Este fluxo demonstra a clara separação de responsabilidades na arquitetura.
  ENDPOINT("GET", "/orders", listByCpf, QUERY(String, cpf)) {
    if (!cpf || cpf->empty()) {
      return createResponse(Status::CODE_400, "cpf é obrigatório");
    }
    auto details = orderService_->listDetailsByCpf(cpf->c_str());

    auto arr = oatpp::List<oatpp::Object<OrderOutDto>>::createShared();
    for (const auto& d : details) {
      arr->push_back(toOutDto(d));
    }
    return createDtoResponse(Status::CODE_200, arr);
  }
};

#include OATPP_CODEGEN_END(ApiController)
