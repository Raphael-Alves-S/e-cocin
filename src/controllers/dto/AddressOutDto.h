#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)
class AddressOutDto : public oatpp::DTO {
  DTO_INIT(AddressOutDto, DTO)
  DTO_FIELD(Int64, id);
  DTO_FIELD(Int64, clientId);
  DTO_FIELD(String, street);
  DTO_FIELD(String, number);
  DTO_FIELD(String, city);
  DTO_FIELD(String, state);
  DTO_FIELD(String, zip);
  DTO_FIELD(String, addressType);
  DTO_FIELD(Int64, createDate);
};

#include OATPP_CODEGEN_END(DTO)