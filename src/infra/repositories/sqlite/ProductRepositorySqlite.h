#ifndef ECOCIN_INFRA_REPOSITORIES_SQLITE_PRODUCTREPOSITORYSQLITE_H
#define ECOCIN_INFRA_REPOSITORIES_SQLITE_PRODUCTREPOSITORYSQLITE_H

#include "domain/repositories/IProductRepository.h"
#include "infra/db/SqliteConnection.h"

namespace ecocin::infra::repositories::sqlite {

class ProductRepositorySqlite : public ecocin::domain::repositories::IProductRepository {
private:
    ecocin::infra::db::SqliteConnection& connection_;

public:
    explicit ProductRepositorySqlite(ecocin::infra::db::SqliteConnection& connection) : connection_(connection) {}

    Product create(const Product& in) override;
    std::optional<Product> findById(long long id) override;
    std::optional<Product> findBySku(const std::string& sku) override;
    std::vector<Product> listAll() override;
    bool update(const Product& p) override;
    bool remove(long long id) override;
};

}

#endif
