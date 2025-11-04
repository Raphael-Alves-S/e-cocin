#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class AddressBriefDto : public oatpp::DTO {
  DTO_INIT(AddressBriefDto, DTO)
  DTO_FIELD(String, street);
  DTO_FIELD(String, number);
  DTO_FIELD(String, city);
  DTO_FIELD(String, state);
  DTO_FIELD(String, zip);
  DTO_FIELD(String, addressType);
};

#include OATPP_CODEGEN_END(DTO)
