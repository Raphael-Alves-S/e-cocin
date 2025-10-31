#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

class ClientDto : public oatpp::DTO {
  DTO_INIT(ClientDto, DTO)
  DTO_FIELD(String, name);
  DTO_FIELD(String, email);
  DTO_FIELD(String, cpf);
};

#include OATPP_CODEGEN_END(DTO)
