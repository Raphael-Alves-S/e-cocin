#ifndef ECOCIN_INFRA_REPOSITORIES_IPRODUCTREPOSITORY_H
#define ECOCIN_INFRA_REPOSITORIES_IPRODUCTREPOSITORY_H

#include <vector>
#include <optional>
#include "../../domain/entities/Product.h"

namespace ecocin::domain::repositories {

class IProductRepository {
public:
    virtual ~IProductRepository() = default;

    virtual Product create(const Product& in) = 0;
    virtual std::optional<Product> findById(long long id) = 0;
    virtual std::vector<Product> listAll() = 0;
    virtual bool update(const Product& p) = 0;
    virtual bool remove(long long id) = 0;
};

} // namespace ecocin::infra::repositories

#endif // ECOCIN_INFRA_REPOSITORIES_IPRODUCTREPOSITORY_H
