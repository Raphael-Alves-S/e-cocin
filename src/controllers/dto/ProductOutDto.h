#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ProductOutDto : public oatpp::DTO {
  DTO_INIT(ProductOutDto, DTO)

  DTO_FIELD(Int64,    id);
  DTO_FIELD(String,   name);
  DTO_FIELD(String,   description);
  DTO_FIELD(String,   sku);
  DTO_FIELD(Float64,  price);
  DTO_FIELD(Int32,    stockQuantity);
  DTO_FIELD(Boolean,  isActive);
  DTO_FIELD(Int64,    createDate);
};

#include OATPP_CODEGEN_END(DTO)