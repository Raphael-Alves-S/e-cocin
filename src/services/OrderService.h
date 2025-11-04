#ifndef ORDER_SERVICE_H
#define ORDER_SERVICE_H

#include <string>
#include <optional>
#include <vector>

#include "../domain/entities/Order.h"
#include "../domain/entities/Client.h"
#include "../domain/entities/Product.h"
#include "../domain/entities/Address.h"
#include "../infra/repositories/sqlite/OrderRepositorySqlite.h"
#include "../infra/repositories/sqlite/ClientRepositorySqlite.h"
#include "../infra/repositories/sqlite/ProductRepositorySqlite.h"
#include "../infra/repositories/sqlite/AddressRepositorySqlite.h"

namespace ecocin::services {

struct OrderDetails {
  Order order;
  Client client;
  Product product;
  Address address;
};

class OrderService {
public:
  OrderService(
    infra::repositories::sqlite::OrderRepositorySqlite& orderRepo,
    infra::repositories::sqlite::ClientRepositorySqlite& clientRepo,
    infra::repositories::sqlite::ProductRepositorySqlite& productRepo,
    infra::repositories::sqlite::AddressRepositorySqlite& addressRepo);

  // Cria pedido com cpf + sku + shippingAddressType (unitPrice e status definidos no backend)
  std::optional<Order> createByCpfSkuAndType(const std::string& cpf,
                                             const std::string& sku,
                                             const std::string& shippingAddressType,
                                             int quantity);

  // Lista pedidos por CPF já com entidades relacionadas
  std::vector<OrderDetails> listDetailsByCpf(const std::string& cpf);

  // Utilidades
  std::optional<Order> getById(long long id);

private:
  infra::repositories::sqlite::OrderRepositorySqlite&   orderRepo_;
  infra::repositories::sqlite::ClientRepositorySqlite&  clientRepo_;
  infra::repositories::sqlite::ProductRepositorySqlite& productRepo_;
  infra::repositories::sqlite::AddressRepositorySqlite& addressRepo_;

  std::optional<Address> resolveAddressForClient(long long clientId,
                                                 const std::string& addressType);
};

} // namespace ecocin::services

#endif
