#ifndef ECOCIN_DOMAIN_REPOSITORIES_ICLIENTREPOSITORY_H
#define ECOCIN_DOMAIN_REPOSITORIES_ICLIENTREPOSITORY_H

#include <vector>
#include <optional>
#include <string>
#include "../../domain/entities/Client.h"

namespace ecocin::domain::repositories {

class IClientRepository {
public:
    virtual ~IClientRepository() = default;

    virtual Client create(const Client& in) = 0;
    virtual std::optional<Client> findById(long long id) = 0;
    virtual std::optional<Client> findByCpf(const std::string& cpf) = 0;
    virtual std::vector<Client> listAll() = 0;
    virtual bool update(const Client& c) = 0;
    virtual bool remove(long long id) = 0;
};

} // namespace ecocin::domain::repositories

#endif
