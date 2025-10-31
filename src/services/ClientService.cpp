#include "ClientService.h"
#include <stdexcept>
#include "../infra/repositories/sqlite/ClientRepositorySqlite.h"

namespace ecocin::services {

    ClientService::ClientService(infra::repositories::sqlite::ClientRepositorySqlite& clientRepo)
        : clientRepo_(clientRepo) {}

    std::string ClientService::createClient(const Client& client) {
        if (clientExists(client.getCpf())) {
            throw std::runtime_error("Client with CPF already exists");
        }
        clientRepo_.create(client);
        return "Client created successfully";
    }

    std::optional<Client> ClientService::getClientByCpf(const std::string& cpf) {
        return clientRepo_.findByCpf(cpf);
    }

    bool ClientService::clientExists(const std::string& cpf) {
        return clientRepo_.findByCpf(cpf).has_value();
    }

    std::vector<Client> ClientService::listAllClients() {
        return clientRepo_.listAll();
    }

    bool ClientService::updateClient(const Client& client) {
        if (!clientExists(client.getCpf())) {
            throw std::runtime_error("Client does not exist");
        }
        return clientRepo_.update(client);
    }

    std::string ClientService::removeClientMessage(const std::string& cpf) {
        auto found = clientRepo_.findByCpf(cpf);
        if (!found) {
            throw std::runtime_error("Client does not exist");
        }
        clientRepo_.remove(found->getId());   // <-- remove por id
        return "Client removed successfully";
    }

}
