#include "OrderService.h"
#include <sstream>

using namespace ecocin;
using ecocin::services::OrderService;
using ecocin::services::OrderDetails;

OrderService::OrderService(
  infra::repositories::sqlite::OrderRepositorySqlite& orderRepo,
  infra::repositories::sqlite::ClientRepositorySqlite& clientRepo,
  infra::repositories::sqlite::ProductRepositorySqlite& productRepo,
  infra::repositories::sqlite::AddressRepositorySqlite& addressRepo)
  : orderRepo_(orderRepo)
  , clientRepo_(clientRepo)
  , productRepo_(productRepo)
  , addressRepo_(addressRepo) {}

std::optional<Address> OrderService::resolveAddressForClient(long long clientId,
                                                             const std::string& type) {
  // 1) tenta por tipo
  auto list = addressRepo_.listByClientId(clientId);
  Address only{};
  bool hasOnly = false;

  for (const auto& a : list) {
    if (a.getAddressType() == type) {
      return a;
    }
  }
  // 2) fallback: se tiver exatamente 1 endereço, usa-o
  if (list.size() == 1) {
    return list.front();
  }
  // 3) nenhum adequado
  return std::nullopt;
}

std::optional<Order> OrderService::createByCpfSkuAndType(const std::string& cpf,
                                                         const std::string& sku,
                                                         const std::string& shippingAddressType,
                                                         int quantity) {
  if (cpf.empty() || sku.empty() || quantity <= 0) return std::nullopt;

  auto clientOpt = clientRepo_.findByCpf(cpf);
  if (!clientOpt) return std::nullopt;
  const long long clientId = clientOpt->getId();

  auto productOpt = productRepo_.findBySku(sku);
  if (!productOpt) return std::nullopt;
  const long long productId = productOpt->getId();
  const double unitPrice = productOpt->getPrice(); // vem do produto

  auto addrOpt = resolveAddressForClient(clientId, shippingAddressType);
  if (!addrOpt) {
    // sem endereço compatível — se quiser, retorne std::nullopt e o controller manda 400 com msg clara
    return std::nullopt;
  }

  Order o(clientId, productId, addrOpt->getId(), quantity, unitPrice, "PENDING");
  o.calculateTotal();
  return orderRepo_.create(o);
}

std::optional<Order> OrderService::getById(long long id) {
  return orderRepo_.findById(id);
}

std::vector<OrderDetails> OrderService::listDetailsByCpf(const std::string& cpf) {
  std::vector<OrderDetails> out;
  auto clientOpt = clientRepo_.findByCpf(cpf);
  if (!clientOpt) return out;

  const long long clientId = clientOpt->getId();
  auto orders = orderRepo_.listByClientId(clientId);
  if (orders.empty()) return out;

  // carrega uma vez o cliente
  const Client client = *clientOpt;

  // para cada order, busca product e address
  for (const auto& o : orders) {
    auto productOpt = productRepo_.findById(o.getProductId());
    auto addressOpt = addressRepo_.findById(o.getShippingAddressId());
    if (!productOpt || !addressOpt) continue;

    out.push_back(OrderDetails{ o, client, *productOpt, *addressOpt });
  }
  return out;
}
