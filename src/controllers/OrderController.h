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

class OrderController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::OrderService> orderService_;

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
  OrderController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                  std::shared_ptr<ecocin::services::OrderService> service)
    : oatpp::web::server::api::ApiController(objectMapper),
      orderService_(std::move(service)) {}

  // POST /orders
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

  // GET /orders?cpf=...
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
