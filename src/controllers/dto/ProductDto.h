#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class ProductDto : public oatpp::DTO {
    DTO_INIT(ProductDto, DTO)

    DTO_FIELD(String, id);
    DTO_FIELD(String, name);
    DTO_FIELD(String, description);
    DTO_FIELD(Int64, price);
    DTO_FIELD(Int32, stockQuantity);
    DTO_FIELD(Boolean, isActive);
    DTO_FIELD(String, sku);
};

#include OATPP_CODEGEN_END(DTO)
