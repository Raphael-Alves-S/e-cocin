#pragma once
#include "oatpp/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/data/type/Type.hpp" 
#include "../services/ProductService.h"
#include "dto/ProductDto.h"
#include "dto/ProductOutDto.h"
#include <memory>

#include OATPP_CODEGEN_BEGIN(ApiController)


class ProductController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::ProductService> productService;

  public:
  ProductController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                    std::shared_ptr<ecocin::services::ProductService> service)
    : oatpp::web::server::api::ApiController(objectMapper),
      productService(std::move(service)) {}

  static oatpp::Object<ProductOutDto> toOutDto(const Product& p) {
    auto dto = ProductOutDto::createShared();
    dto->id            = p.getId();
    dto->name          = oatpp::String(p.getName().c_str());
    dto->description   = oatpp::String(p.getDescription().c_str());
    dto->sku           = oatpp::String(p.getSku().str().c_str());
    dto->price         = p.getPrice();                 // double
    dto->stockQuantity = p.getStockQuantity();
    dto->isActive      = p.getIsActive();

    using namespace std::chrono;
    dto->createDate = duration_cast<seconds>(p.getCreateDate().time_since_epoch()).count();
    return dto;
  }


  // POST /products
  ENDPOINT("POST", "/products", createProduct, BODY_DTO(Object<ProductDto>, body)) {
  Product p;
  if (body->name)          p.setName(std::string(body->name->c_str()));
  if (body->description)   p.setDescription(std::string(body->description->c_str()));
  if (body->price)         p.setPrice(*body->price);
  if (body->stockQuantity) p.setStockQuantity(*body->stockQuantity);
  if (body->isActive)      p.setIsActive(*body->isActive);
  if (body->sku)           p.setSku(ecocin::core::Uuid(std::string(body->sku->c_str())));

  auto msg = productService->createProduct(p);
  return createResponse(Status::CODE_200, oatpp::String(msg.c_str()));
}

  // GET /products
  ENDPOINT("GET", "/products", listAll) {
  const auto items = productService->listAll();

  auto arr = oatpp::List<oatpp::Object<ProductOutDto>>::createShared();
  for (const auto& p : items) {
    arr->push_back(toOutDto(p));
  }
  return createDtoResponse(Status::CODE_200, arr);
}

  // GET /products/sku/{sku}
  ENDPOINT("GET", "/products/sku/{sku}", getBySku, PATH(String, sku)) {
    const std::string key = sku ? std::string(sku->c_str()) : std::string{};
    auto p = productService->getBySku(key);
    if (!p) {
      return createResponse(Status::CODE_404, "Product not found");
    }
    return createDtoResponse(Status::CODE_200, toOutDto(*p));
  }

  // GET /products/{id}
  ENDPOINT("GET", "/products/{id}", getById, PATH(Int64, id)) {
    auto p = productService->getById(id);
    if (!p) {
      return createResponse(Status::CODE_404, "Product not found");
    }
    return createDtoResponse(Status::CODE_200, toOutDto(*p));
  }

  // PUT /products/{id}
  ENDPOINT("PUT", "/products/{id}", updateProduct,
         PATH(Int64, id),
         BODY_DTO(Object<ProductDto>, body)) {
  auto existing = productService->getById(id);
  if (!existing) return createResponse(Status::CODE_404, "Product not found");

  Product p = *existing;
  if (body->name)          p.setName(std::string(body->name->c_str()));
  if (body->description)   p.setDescription(std::string(body->description->c_str()));
  if (body->price)         p.setPrice(*body->price);
  if (body->stockQuantity) p.setStockQuantity(*body->stockQuantity);
  if (body->isActive)      p.setIsActive(*body->isActive);
  if (body->sku)           p.setSku(ecocin::core::Uuid(std::string(body->sku->c_str())));

  if (!productService->updateProduct(p)) {
    return createResponse(Status::CODE_400, "Invalid product data");
  }
  return createDtoResponse(Status::CODE_200, toOutDto(p));
}

  // DELETE /products/{id}
  ENDPOINT("DELETE", "/products/{id}", deleteById, PATH(Int64, id)) {
    auto msg = productService->removeByIdMessage(id);
    if (msg == "Product not found") {
      return createResponse(Status::CODE_404, oatpp::String(msg.c_str()));
    }
    return createResponse(Status::CODE_200, oatpp::String(msg.c_str()));
  }
};

#include OATPP_CODEGEN_END(ApiController)