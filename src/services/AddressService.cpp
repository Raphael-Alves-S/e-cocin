#include "AddressService.h"
#include <sstream>

using namespace ecocin;
using ecocin::infra::repositories::sqlite::AddressRepositorySqlite;
using ecocin::infra::repositories::sqlite::ClientRepositorySqlite;

namespace ecocin::services {
    // O construtor utiliza injeção de dependência para receber instâncias dos repositórios.
    // Este é um pilar do design de software SOLID (Inversão de Dependência),
    // que desacopla a camada de serviço da implementação concreta do acesso a dados,
    // facilitando a testabilidade e a manutenção.
    AddressService::AddressService(AddressRepositorySqlite& addressRepo,
                                   ClientRepositorySqlite& clientRepo)
      : addrRepo_(addressRepo), clientRepo_(clientRepo) {}

    // Cria um novo endereço associado a um cliente, identificado pelo CPF.
    // Este método orquestra a lógica de negócio: primeiro, valida a existência do cliente
    // e, em seguida, delega a criação do endereço ao repositório correspondente.
    // A camada de serviço atua como uma fachada, simplificando operações complexas.
    std::optional<Address> AddressService::create(const std::optional<std::string>& cpf,
                                                  const Address& in) {

        auto clientOpt = clientRepo_.findByCpf(cpf.value());
        if (!clientOpt.has_value()) {
            return std::nullopt;
        }

        Address addr = in;
        addr.setClientId(clientOpt->getId());
        Address createdAddr = addrRepo_.create(addr);
        return createdAddr;
    }

    // Busca um endereço pelo seu ID.
    // O método simplesmente delega a chamada para o repositório,
    // atuando como um intermediário que mantém a arquitetura em camadas consistente.
    // A lógica de negócio aqui é mínima, mas a camada de serviço ainda é importante
    // para isolar o controller do acesso direto aos dados.
    std::optional<Address> AddressService::getById(long long id) {
        return addrRepo_.findById(id);
    }

    // Lista todos os endereços cadastrados.
    // Assim como `getById`, este método é um simples "pass-through" para o repositório,
    // mantendo a separação de responsabilidades entre as camadas.
    std::vector<Address> AddressService::listAll() {
        return addrRepo_.listAll();
    }

    // Lista todos os endereços de um cliente específico, buscando-o pelo CPF.
    // Este método exemplifica a orquestração entre diferentes repositórios:
    // primeiro, utiliza o `clientRepo_` para encontrar o cliente e, em seguida,
    // o `addrRepo_` para buscar os endereços associados.
    std::vector<Address> AddressService::listByCpf(const std::string& cpf) {
        if (cpf.empty()) return {};
        auto cli = clientRepo_.findByCpf(cpf);
        if (!cli) return {};
        return addrRepo_.listByClientId(cli->getId());
    }


    // Atualiza um endereço existente.
    // A lógica de negócio aqui inclui uma verificação de existência antes da atualização,
    // garantindo que não se tente modificar um registro que não existe.
    // O método preserva a data de criação original, uma regra de negócio importante.
    bool AddressService::update(const long long id, const Address& in) {
        auto existingAddrOpt = addrRepo_.findById(id);
        if (!existingAddrOpt.has_value()) {
            return false;
        }
        Address updatedAddr = in;
        updatedAddr.setId(id);
        updatedAddr.setCreateDate(existingAddrOpt->getCreateDate());
        return addrRepo_.update(updatedAddr);
    }

    // Remove um endereço do sistema.
    // A camada de serviço delega a operação de exclusão diretamente para o repositório,
    // encapsulando a lógica de acesso a dados e fornecendo uma interface limpa
    // para a camada de apresentação (controllers).
    bool AddressService::remove(long long id) {
        return addrRepo_.remove(id);
    }
}
