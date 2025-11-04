#include "ProductService.h"
#include <sstream>

using namespace ecocin;
using ecocin::infra::repositories::sqlite::ProductRepositorySqlite;

namespace ecocin::services {

// O construtor aplica o princípio da Inversão de Dependência, recebendo o repositório
// de produtos como uma dependência externa. Isso torna o serviço mais testável e flexível,
// pois ele não está acoplado a uma implementação concreta de acesso a dados.
ProductService::ProductService(ProductRepositorySqlite& productRepo)
  : productRepo_(productRepo) {}

// Valida as regras de negócio de um objeto 'Product'.
// Este método privado encapsula a lógica de validação (campos obrigatórios, valores válidos),
// promovendo a reutilização de código e mantendo os métodos públicos, como 'create' e 'update',
// mais limpos e focados em sua responsabilidade principal.
std::string ProductService::validate(const Product& p) {
  std::ostringstream oss;
  if (p.getName().empty())        oss << "name is required; ";
  if (p.getPrice() < 0.0)         oss << "price must be >= 0; ";
  if (p.getStockQuantity() < 0)   oss << "stockQuantity must be >= 0; ";
  return oss.str();
}

// Orquestra a criação de um novo produto.
// A camada de serviço implementa várias regras de negócio aqui:
// 1. Gera um SKU único se não for fornecido.
// 2. Valida a integridade dos dados do produto.
// 3. Garante que não existam produtos com SKU duplicado.
// Somente após essas verificações, a persistência é delegada ao repositório.
std::string ProductService::createProduct(const Product& in) {
  Product p = in;

  // SKU: se vier vazio, geramos um novo UUID
  if (p.getSku().empty()) {
    p.setSku(core::Uuid::v4());
  }

  const auto errors = validate(p);
  if (!errors.empty()) {
    return std::string("Invalid product: ") + errors;
  }

  // regra de negócio: não permitir SKU duplicado
  if (existsBySku(p.getSku().str())) {
    return "Duplicated SKU";
  }

  productRepo_.create(p);
  return "Product created";
}

// Busca um produto pelo seu ID.
// O serviço atua como uma fachada, delegando a chamada diretamente ao repositório.
// Manter essa camada de passagem é importante para a consistência da arquitetura.
std::optional<Product> ProductService::getById(long long id) {
  return productRepo_.findById(id);
}

// Busca um produto pelo seu SKU.
// Expõe uma funcionalidade de busca por uma chave de negócio, o que é uma
// responsabilidade comum da camada de serviço para abstrair as necessidades da aplicação.
std::optional<Product> ProductService::getBySku(const std::string& sku) {
  return productRepo_.findBySku(sku);
}

// Retorna uma lista com todos os produtos.
// Delega a chamada para o repositório, fornecendo uma interface simples e
// consistente para a camada de apresentação.
std::vector<Product> ProductService::listAll() {
  return productRepo_.listAll();
}

// Atualiza um produto existente.
// Antes de delegar a atualização para o repositório, o serviço executa a mesma
// lógica de validação da criação, garantindo a consistência e integridade dos dados.
bool ProductService::updateProduct(const Product& p) {
  const auto errors = validate(p);
  if (!errors.empty()) {
    return false;
  }
  return productRepo_.update(p);
}

// Remove um produto pelo seu ID.
// A lógica de negócio aqui é verificar se o produto existe antes de tentar removê-lo,
// fornecendo uma mensagem de retorno clara sobre o resultado da operação.
std::string ProductService::removeByIdMessage(long long id) {
  auto found = productRepo_.findById(id);
  if (!found) return "Product not found";
  return productRepo_.remove(id) ? "Product removed" : "Could not remove product";
}

// Verifica a existência de um produto com base no SKU.
// Este método auxiliar é útil para lógicas de negócio, como a de criação,
// para evitar a duplicação de registros com o mesmo identificador de negócio.
bool ProductService::existsBySku(const std::string& sku) {
  return static_cast<bool>(productRepo_.findBySku(sku));
}

} // namespace ecocin::services
