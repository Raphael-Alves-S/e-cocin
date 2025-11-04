#ifndef ECOCIN_SERVICES_PRODUCTSERVICE_H
#define ECOCIN_SERVICES_PRODUCTSERVICE_H

#include "../domain/entities/Product.h"
#include "../infra/repositories/sqlite/ProductRepositorySqlite.h"
#include "../domain/core/Uuid.h"
#include <optional>
#include <string>
#include <vector>

namespace ecocin::services {

// Classe de serviço para gerenciar operações relacionadas a produtos
class ProductService {
public:
  explicit ProductService(ecocin::infra::repositories::sqlite::ProductRepositorySqlite& productRepo);


  std::string createProduct(const Product& in);
  std::optional<Product> getById(long long id);
  std::optional<Product> getBySku(const std::string& sku);
  std::vector<Product> listAll();

  bool updateProduct(const Product& p);
  std::string removeByIdMessage(long long id);

  bool existsBySku(const std::string& sku);

private:
  ecocin::infra::repositories::sqlite::ProductRepositorySqlite& productRepo_;
  static std::string validate(const Product& p);
};

} // namespace ecocin::services

#endif // ECOCIN_SERVICES_PRODUCTSERVICE_H
