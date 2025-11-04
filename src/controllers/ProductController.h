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


// O ProductController é a camada de entrada para todas as requisições HTTP
// relacionadas a produtos. Ele implementa o padrão Controller do MVC, sendo responsável
// por receber as requisições, interagir com a camada de serviço para executar
// a lógica de negócio e retornar uma resposta formatada (geralmente JSON).
class ProductController : public oatpp::web::server::api::ApiController {
private:
  std::shared_ptr<ecocin::services::ProductService> productService;

  public:
  // O construtor adota o padrão de Injeção de Dependência para receber o serviço de produto.
  // Isso desacopla o controller da implementação concreta do serviço, o que é um
  // pilar da arquitetura SOLID, facilitando a manutenção e os testes unitários.
  ProductController(const std::shared_ptr<oatpp::json::ObjectMapper>& objectMapper,
                    std::shared_ptr<ecocin::services::ProductService> service)
    : oatpp::web::server::api::ApiController(objectMapper),
      productService(std::move(service)) {}

  // Converte um objeto de domínio 'Product' para um 'ProductOutDto' (Data Transfer Object).
  // O uso de DTOs é uma forma de encapsulamento que protege a estrutura interna do domínio,
  // permitindo que a API exponha apenas os dados relevantes e no formato desejado,
  // sem criar um acoplamento forte com o modelo interno.
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


  // Endpoint para criar um novo produto.
  // Ele extrai os dados do corpo da requisição (BODY_DTO), constrói um objeto de domínio 'Product'
  // e o passa para a camada de serviço, que contém as regras de negócio (validação, etc.).
  // A responsabilidade do controller é gerenciar o fluxo HTTP, não a lógica de negócio.
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

  // Endpoint para listar todos os produtos.
  // O controller delega a busca ao serviço, recebe a lista de produtos,
  // converte cada um para seu DTO correspondente e retorna a coleção na resposta HTTP.
  ENDPOINT("GET", "/products", listAll) {
  const auto items = productService->listAll();

  auto arr = oatpp::List<oatpp::Object<ProductOutDto>>::createShared();
  for (const auto& p : items) {
    arr->push_back(toOutDto(p));
  }
  return createDtoResponse(Status::CODE_200, arr);
}

  // Endpoint para buscar um produto pelo seu SKU (identificador de negócio).
  // Ele extrai o SKU da URL, chama o serviço e trata os dois possíveis resultados:
  // sucesso (retorna 200 OK com o DTO do produto) ou falha (retorna 404 Not Found).
  ENDPOINT("GET", "/products/sku/{sku}", getBySku, PATH(String, sku)) {
    const std::string key = sku ? std::string(sku->c_str()) : std::string{};
    auto p = productService->getBySku(key);
    if (!p) {
      return createResponse(Status::CODE_404, "Product not found");
    }
    return createDtoResponse(Status::CODE_200, toOutDto(*p));
  }

  // Endpoint para buscar um produto pelo seu ID técnico.
  // A lógica é similar à busca por SKU, demonstrando como o controller
  // pode expor diferentes formas de acessar o mesmo recurso.
  ENDPOINT("GET", "/products/{id}", getById, PATH(Int64, id)) {
    auto p = productService->getById(id);
    if (!p) {
      return createResponse(Status::CODE_404, "Product not found");
    }
    return createDtoResponse(Status::CODE_200, toOutDto(*p));
  }

  // Endpoint para atualizar um produto existente.
  // Este método orquestra uma operação mais complexa: primeiro, busca o produto
  // existente para garantir que ele existe (retornando 404 se não), depois aplica as
  // modificações do corpo da requisição e, por fim, chama o serviço para validar e persistir.
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

  // Endpoint para remover um produto pelo ID.
  // O controller invoca o serviço para realizar a exclusão e retorna uma resposta
  // apropriada com base no feedback do serviço (sucesso na remoção ou produto não encontrado).
  ENDPOINT("DELETE", "/products/{id}", deleteById, PATH(Int64, id)) {
    auto msg = productService->removeByIdMessage(id);
    if (msg == "Product not found") {
      return createResponse(Status::CODE_404, oatpp::String(msg.c_str()));
    }
    return createResponse(Status::CODE_200, oatpp::String(msg.c_str()));
  }
};

#include OATPP_CODEGEN_END(ApiController)
