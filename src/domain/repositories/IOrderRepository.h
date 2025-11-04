#ifndef ECOCIN_DOMAIN_REPOSITORIES_IORDERREPOSITORY_H
#define ECOCIN_DOMAIN_REPOSITORIES_IORDERREPOSITORY_H

#include <vector>
#include <optional>
#include <string>
#include "../../domain/entities/Order.h"

namespace ecocin::domain::repositories {

class IOrderRepository {
public:
  virtual ~IOrderRepository() = default;

  virtual Order create(const Order& in) = 0;
  virtual std::optional<Order> findById(long long id) = 0;
  virtual std::vector<Order> listAll() = 0;
  virtual bool update(const Order& o) = 0;
  virtual bool remove(long long id) = 0;

  // Consultas
  virtual std::vector<Order> listByClientId(long long clientId) = 0;
  virtual bool updateStatus(long long id, const std::string& newStatus) = 0;
  virtual bool updateShippingAddress(long long id, long long newAddressId) = 0;
};

} // namespace ecocin::domain::repositories

#endif // ECOCIN_DOMAIN_REPOSITORIES_IORDERREPOSITORY_H
