#ifndef ECOCIN_INFRA_REPOSITORIES_SQLITE_PRODUCTREPOSITORYSQLITE_H
#define ECOCIN_INFRA_REPOSITORIES_SQLITE_PRODUCTREPOSITORYSQLITE_H

#include "../../domain/repositories/IProductRepository.h"
#include "../../db/SqliteConnection.h"

namespace ecocin::infra::repositories::sqlite {

class ProductRepositorySqlite : public IProductRepository {
private:
    ecocin::infra::db::SqliteConnection& cx_;

public:
    explicit ProductRepositorySqlite(ecocin::infra::db::SqliteConnection& c) : cx_(c) {}

    Product create(const Product& in) override;
    std::optional<Product> findById(long long id) override;
    std::vector<Product> listAll() override;
    bool update(const Product& p) override;
    bool remove(long long id) override;
};

} // namespace ecocin::infra::repositories::sqlite

#endif // ECOCIN_INFRA_REPOSITORIES_SQLITE_PRODUCTREPOSITORYSQLITE_H
