#include "ProductService.h"
#include <sstream>

using namespace ecocin;
using ecocin::infra::repositories::sqlite::ProductRepositorySqlite;

namespace ecocin::services {

ProductService::ProductService(ProductRepositorySqlite& productRepo)
  : productRepo_(productRepo) {}

std::string ProductService::validate(const Product& p) {
  std::ostringstream oss;
  if (p.getName().empty())        oss << "name is required; ";
  if (p.getPrice() < 0.0)         oss << "price must be >= 0; ";
  if (p.getStockQuantity() < 0)   oss << "stockQuantity must be >= 0; ";
  return oss.str();
}

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

std::optional<Product> ProductService::getById(long long id) {
  return productRepo_.findById(id);
}

std::optional<Product> ProductService::getBySku(const std::string& sku) {
  return productRepo_.findBySku(sku);
}

std::vector<Product> ProductService::listAll() {
  return productRepo_.listAll();
}

bool ProductService::updateProduct(const Product& p) {
  const auto errors = validate(p);
  if (!errors.empty()) {
    return false;
  }
  return productRepo_.update(p);
}

std::string ProductService::removeByIdMessage(long long id) {
  auto found = productRepo_.findById(id);
  if (!found) return "Product not found";
  return productRepo_.remove(id) ? "Product removed" : "Could not remove product";
}

bool ProductService::existsBySku(const std::string& sku) {
  return static_cast<bool>(productRepo_.findBySku(sku));
}

} // namespace ecocin::services
