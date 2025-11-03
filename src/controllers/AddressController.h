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

class AddressController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::AddressService> addressService;

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
  AddressController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                    std::shared_ptr<ecocin::services::AddressService> service)
  : oatpp::web::server::api::ApiController(objectMapper),
    addressService(std::move(service)) {}

  // POST /addresses  (CPF obrigatório)
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
    return createDtoResponse(Status::CODE_201, toOutDto(*created));
  }

  // GET /addresses
  ENDPOINT("GET", "/addresses", listAll) {
    const auto items = addressService->listAll();
    auto arr = oatpp::List<oatpp::Object<AddressOutDto>>::createShared();
    for (const auto& addr : items) {
      arr->push_back(toOutDto(addr));
    }
    return createDtoResponse(Status::CODE_200, arr);
  }

  // GET /clients/{cpf}/addresses
  ENDPOINT("GET", "/clients/{cpf}/addresses", listAddressesByCpf,
           PATH(String, cpf)) {
    auto list = addressService->listByCpf(std::string(cpf->c_str()));
    auto arr = oatpp::List<oatpp::Object<AddressOutDto>>::createShared();
    for (const auto& a : list) arr->push_back(toOutDto(a));
    return createDtoResponse(Status::CODE_200, arr);
  }
};

#include OATPP_CODEGEN_END(ApiController)
