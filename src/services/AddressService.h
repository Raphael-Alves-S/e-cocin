#ifndef ADDRESS_SERVICE_H
#define ADDRESS_SERVICE_H
#include <string>
#include <optional>
#include <vector>

#include "../domain/entities/Address.h"
#include "../infra/repositories/sqlite/AddressRepositorySqlite.h" 
#include "../infra/repositories/sqlite/ClientRepositorySqlite.h"


namespace ecocin::services {
    class AddressService {
    public:
        explicit AddressService(ecocin::infra::repositories::sqlite::AddressRepositorySqlite& addressRepo,
                                ecocin::infra::repositories::sqlite::ClientRepositorySqlite& clientRepo);
        std::optional<Address> create(const std::optional<std::string>& cpf,
                                const Address& in);
        std::optional<Address> getById(long long id);
        std::vector<Address> listAll();
        bool update(const long long id, const Address& in);
        bool remove(long long id);
        std::vector<Address> listByCpf(const std::string& cpf);

    private:
        infra::repositories::sqlite::AddressRepositorySqlite& addrRepo_;
        infra::repositories::sqlite::ClientRepositorySqlite& clientRepo_;
};
}
#endif