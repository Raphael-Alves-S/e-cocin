#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Type.hpp"


#include OATPP_CODEGEN_BEGIN(DTO)

class ProductDto : public oatpp::DTO {

    DTO_INIT(ProductDto, DTO)

    DTO_FIELD(String, id);
    DTO_FIELD(String, name);
    DTO_FIELD(String, description);
    DTO_FIELD(Int64, priceCents);
    DTO_FIELD(Int32, stock);
    DTO_FIELD(String, status);

};

#include OATPP_CODEGEN_END(DTO)
