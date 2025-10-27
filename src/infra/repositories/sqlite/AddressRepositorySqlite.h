#ifndef ECOCIN_INFRA_REPOSITORIES_SQLITE_ADDRESSREPOSITORYSQLITE_H
#define ECOCIN_INFRA_REPOSITORIES_SQLITE_ADDRESSREPOSITORYSQLITE_H

#include "../../domain/repositories/IAddressRepository.h"
#include "../../db/SqliteConnection.h"

namespace ecocin::infra::repositories::sqlite {

class AddressRepositorySqlite : public IAddressRepository {
private:
    ecocin::infra::db::SqliteConnection& connection_;
public:
    explicit AddressRepositorySqlite(ecocin::infra::db::SqliteConnection& connection) : connection_(connection) {}

    Address create(const Address& in) override;
    std::optional<Address> findById(long long id) override;
    std::vector<Address> listAll() override;
    bool update(const Address& addr) override;
    bool remove(long long id) override;
};
}
#endif // ECOCIN_INFRA_REPOSITORIES_SQLITE_ADDRESSREPOSITORYSQLITE_H
