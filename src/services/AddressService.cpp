#include "AddressService.h"
#include <sstream>

using namespace ecocin;
using ecocin::infra::repositories::sqlite::AddressRepositorySqlite;
using ecocin::infra::repositories::sqlite::ClientRepositorySqlite;

namespace ecocin::services {
    AddressService::AddressService(AddressRepositorySqlite& addressRepo,
                                   ClientRepositorySqlite& clientRepo)
      : addrRepo_(addressRepo), clientRepo_(clientRepo) {}

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

    std::optional<Address> AddressService::getById(long long id) {
        return addrRepo_.findById(id);
    }

    std::vector<Address> AddressService::listAll() {
        return addrRepo_.listAll();
    }

    std::vector<Address> AddressService::listByCpf(const std::string& cpf) {
        if (cpf.empty()) return {};
        auto cli = clientRepo_.findByCpf(cpf);
        if (!cli) return {};
        return addrRepo_.listByClientId(cli->getId());
    }


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

    bool AddressService::remove(long long id) {
        return addrRepo_.remove(id);
    }
}