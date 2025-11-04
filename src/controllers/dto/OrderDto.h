#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class OrderDto : public oatpp::DTO {
  DTO_INIT(OrderDto, DTO)

  DTO_FIELD(String, cpf);               // obrigatório no service
  DTO_FIELD(String, sku);               // obrigatório no service
  DTO_FIELD(String, shippingAddressType); // obrigatório no service
  DTO_FIELD(Int32,  quantity);          // padrão: 1 (se controller/service quiser)
};

#include OATPP_CODEGEN_END(DTO)
