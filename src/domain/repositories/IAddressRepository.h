#ifndef IADDRESSREPOSITORY_H
#define IADDRESSREPOSITORY_H
#include <vector>
#include <optional>
#include "../../domain/entities/Address.h"

namespace ecocin::domain::repositories {

class IAddressRepository {
public:
    virtual ~IAddressRepository() = default;

    virtual Address create(const Address& in) = 0;
    virtual std::optional<Address> findById(long long id) = 0;
    virtual std::vector<Address> listAll() = 0;
    virtual bool update(const Address& addr) = 0;
    virtual bool remove(long long id) = 0;
    virtual std::vector<Address>   listByClientId(long long clientId) = 0;
};
}
#endif // IADDRESSREPOSITORY_H