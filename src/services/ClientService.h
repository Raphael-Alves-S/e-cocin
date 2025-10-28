#ifndef ECOCIN_SERVICES_CLIENTSERVICE_H
#define ECOCIN_SERVICES_CLIENTSERVICE_H 

#include "../infra/repositories/sqlite/ClientRepositorySqlite.h"

namespace ecocin::services {
    class ClientService {
    private:
        infra::repositories::sqlite::ClientRepositorySqlite& clientRepo_;

    public:
        explicit ClientService(infra::repositories::sqlite::ClientRepositorySqlite& clientRepo)
            : clientRepo_(clientRepo) {}


       std::string createClient(const Client& client) {
            if (clientExists(client.getCpf())) {
                throw std::runtime_error("Client with CPF already exists");
            }
            clientRepo_.create(client);
            return "Client created successfully";

        }

        std::optional<Client> getClientById(long long id) {
            return clientRepo_.findById(id);
        }

      std::optional<Client> getClientByCpf(const std::string& cpf) {
            return clientRepo_.findByCpf(cpf);
        }

       bool clientExists(const std::string& cpf) {
            return clientRepo_.findByEmail(email).has_value();
        }

        std::vector<Client> listAllClients() {
            return clientRepo_.listAll();
        }

        bool updateClient(const Client& client) {
            return clientRepo_.update(client);
        }

        bool removeClient(long long id) {
            return clientRepo_.remove(id);
        }
    };
}

#endif // ECOCIN_SERVICES_CLIENTSERVICE_H
