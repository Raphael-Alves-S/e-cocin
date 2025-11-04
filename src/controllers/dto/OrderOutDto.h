#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"
#include "AddressBriefDto.h"

#include OATPP_CODEGEN_BEGIN(DTO)

class OrderOutDto : public oatpp::DTO {
  DTO_INIT(OrderOutDto, DTO)

  DTO_FIELD(Int64,    id);
  DTO_FIELD(String,   clientName);
  DTO_FIELD(String,   productDescription);
  DTO_FIELD(Object<AddressBriefDto>, shippingAddress);
  DTO_FIELD(Int32,   quantity);
  DTO_FIELD(Float64, unitPrice);
  DTO_FIELD(Float64, totalPrice);
  DTO_FIELD(String,  status);
  // seguindo o padrão de datas epoch em segundos
  DTO_FIELD(Int64,   createDate);
};

#include OATPP_CODEGEN_END(DTO)
