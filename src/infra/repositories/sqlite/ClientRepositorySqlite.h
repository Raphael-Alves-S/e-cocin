#ifndef ECOCIN_INFRA_REPOSITORIES_SQLITE_CLIENTREPOSITORYSQLITE_H
#define ECOCIN_INFRA_REPOSITORIES_SQLITE_CLIENTREPOSITORYSQLITE_H

#include "domain/repositories/IClientRepository.h"
#include "infra/db/SqliteConnection.h"

namespace ecocin::infra::repositories::sqlite {

// Implementação do repositório de clientes usando SQLite
class ClientRepositorySqlite : public ecocin::domain::repositories::IClientRepository {
private:
    ecocin::infra::db::SqliteConnection& connection_;

public:
    explicit ClientRepositorySqlite(ecocin::infra::db::SqliteConnection& connection) : connection_(connection) {}

    Client create(const Client& in) override;
    std::optional<Client> findById(long long id) override;
    std::optional<Client> findByCpf(const std::string& cpf) override;
    std::vector<Client> listAll() override;
    bool update(const Client& c) override;
    bool remove(long long id) override;
};

} // namespace ecocin::infra::repositories::sqlite

#endif // ECOCIN_INFRA_REPOSITORIES_SQLITE_CLIENTREPOSITORYSQLITE_H
