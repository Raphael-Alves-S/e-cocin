#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ClientOutDto : public oatpp::DTO {
  DTO_INIT(ClientOutDto, DTO)

  DTO_FIELD(Int64,  id);
  DTO_FIELD(String, name);
  DTO_FIELD(String, email);
  DTO_FIELD(String, cpf);
  // epoch seconds (mais simples de serializar)
  DTO_FIELD(Int64,  createDate);
};

#include OATPP_CODEGEN_END(DTO)
