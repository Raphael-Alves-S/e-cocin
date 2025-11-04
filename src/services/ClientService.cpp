#include "ClientService.h"
#include <stdexcept>
#include "../infra/repositories/sqlite/ClientRepositorySqlite.h"

namespace ecocin::services {

    // O construtor implementa a Inversão de Dependência, recebendo uma referência
    // para o repositório de clientes. Isso desacopla o serviço da implementação
    // concreta do acesso a dados, facilitando testes e futuras modificações.
    ClientService::ClientService(infra::repositories::sqlite::ClientRepositorySqlite& clientRepo)
        : clientRepo_(clientRepo) {}

    // Orquestra a criação de um novo cliente.
    // A lógica de negócio principal aqui é validar a unicidade do CPF antes de
    // delegar a persistência ao repositório. Este é um exemplo claro de como a camada
    // de serviço implementa as regras que não pertencem nem à apresentação nem ao acesso a dados.
    std::string ClientService::createClient(const Client& client) {
        if (clientExists(client.getCpf())) {
            throw std::runtime_error("Client with CPF already exists");
        }
        clientRepo_.create(client);
        return "Client created successfully";
    }

    // Busca um cliente pelo CPF.
    // O serviço atua como um intermediário, delegando a chamada diretamente
    // ao repositório e mantendo a arquitetura em camadas coesa.
    std::optional<Client> ClientService::getClientByCpf(const std::string& cpf) {
        return clientRepo_.findByCpf(cpf);
    }

    // Busca um cliente pelo seu ID técnico.
    // Similar à busca por CPF, este método expõe a funcionalidade do repositório
    // através da camada de serviço, garantindo uma interface consistente.
    std::optional<Client> ClientService::getClientById(int64_t id) {
        return clientRepo_.findById(id);
    }

    // Verifica a existência de um cliente com um determinado CPF.
    // Este método auxiliar encapsula a lógica de verificação, tornando o código
    // de outras operações, como a criação, mais limpo e legível.
    bool ClientService::clientExists(const std::string& cpf) {
        return clientRepo_.findByCpf(cpf).has_value();
    }

    // Retorna uma lista de todos os clientes.
    // Delega a responsabilidade ao repositório, mantendo a separação de
    // preocupações e uma arquitetura limpa.
    std::vector<Client> ClientService::listAllClients() {
        return clientRepo_.listAll();
    }

    // Atualiza os dados de um cliente.
    // A lógica de negócio aqui é garantir que o cliente a ser atualizado
    // realmente exista antes de prosseguir com a operação no repositório.
    bool ClientService::updateClient(const Client& client) {
        if (!clientExists(client.getCpf())) {
            throw std::runtime_error("Client does not exist");
        }
        return clientRepo_.update(client);
    }

    // Remove um cliente com base no seu CPF.
    // O serviço primeiro busca o cliente para obter seu ID e depois solicita
    // a remoção ao repositório. Essa orquestração é uma responsabilidade típica
    // da camada de serviço.
    std::string ClientService::removeClientMessage(const std::string& cpf) {
        auto found = clientRepo_.findByCpf(cpf);
        if (!found) {
            throw std::runtime_error("Client does not exist");
        }
        clientRepo_.remove(found->getId());   // <-- remove por id
        return "Client removed successfully";
    }

}
