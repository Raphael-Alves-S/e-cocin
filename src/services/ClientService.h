#ifndef ECOCIN_SERVICES_CLIENTSERVICE_H
#define ECOCIN_SERVICES_CLIENTSERVICE_H 
#include "../domain/entities/Client.h"
#include <string>
#include <vector>
#include <optional>


#include "../infra/repositories/sqlite/ClientRepositorySqlite.h"

namespace ecocin::services {

class ClientService {
public:
    explicit ClientService(ecocin::infra::repositories::sqlite::ClientRepositorySqlite& clientRepo);
    std::string createClient(const Client& client);
    std::optional<Client> getClientByCpf(const std::string& cpf);
    bool clientExists(const std::string& cpf);
    std::vector<Client> listAllClients();
    bool updateClient(const Client& client);
    std::string removeClientMessage(const std::string& cpf);

private:
    ecocin::infra::repositories::sqlite::ClientRepositorySqlite& clientRepo_;
};

}

#endif // ECOCIN_SERVICES_CLIENTSERVICE_H
