#ifndef ECOCIN_INFRA_REPOSITORIES_SQLITE_ORDERREPOSITORYSQLITE_H
#define ECOCIN_INFRA_REPOSITORIES_SQLITE_ORDERREPOSITORYSQLITE_H

#include "domain/repositories/IOrderRepository.h"
#include "infra/db/SqliteConnection.h"
#include <optional>
#include <vector>

namespace ecocin::infra::repositories::sqlite {

class OrderRepositorySqlite : public ecocin::domain::repositories::IOrderRepository {
private:
    ecocin::infra::db::SqliteConnection& connection_;

public:
    explicit OrderRepositorySqlite(ecocin::infra::db::SqliteConnection& connection)
      : connection_(connection) {}

    // IOrderRepository
    Order create(const Order& in) override;
    std::optional<Order> findById(long long id) override;
    std::vector<Order> listAll() override;
    bool update(const Order& o) override;
    bool remove(long long id) override;

    std::vector<Order> listByClientId(long long clientId) override;
    bool updateStatus(long long id, const std::string& newStatus) override;
    bool updateShippingAddress(long long id, long long newAddressId) override;
};

} // namespace ecocin::infra::repositories::sqlite

#endif // ECOCIN_INFRA_REPOSITORIES_SQLITE_ORDERREPOSITORYSQLITE_H
