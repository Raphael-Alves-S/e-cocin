#include "ProductRepositorySqlite.h"
#include "core/Uuid.h"
#include "Helpers.h"

using namespace ecocin;
using infra::repositories::sqlite::ProductRepositorySqlite;

static Product row_to_product(sqlite3_stmt* s) {
    Product product;
    product.setId(static_cast<long long>(sqlite3_column_int64(s, 0)));
    product.setName(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
    if (sqlite3_column_type(s, 2) != SQLITE_NULL)
        product.setDescription(std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 2))));
    product.setSku(ecocin::core::Uuid(reinterpret_cast<const char*>(sqlite3_column_text(s, 3))));
    product.setPrice(sqlite3_column_int64(s, 4));
    product.setStockQuantity(static_cast<int>(sqlite3_column_int(s, 5)));
    product.setIsActive(static_cast<bool>(sqlite3_column_int(s, 6)));
    product.setCreateDate(std::chrono::system_clock::time_point{std::chrono::seconds{sqlite3_column_int64(s, 7)}});
    return product;
}

Product ProductRepositorySqlite::create(const Product& in) {
    Product p = in;
    const char* sql = "INSERT INTO products(name,description,price_cents,status) VALUES(?,?,?,?)";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(cx_.raw(), sql, -1, &st, nullptr), cx_.raw(), "prepare insert product");
    sqlite3_bind_text(st, 1, p.getName().c_str(), -1, SQLITE_TRANSIENT);
    if (p.getDescription().has_value())
        sqlite3_bind_text(st, 2, p.getDescription()->c_str(), -1, SQLITE_TRANSIENT);
    else
        sqlite3_bind_null(st, 2);
    sqlite3_bind_int64(st, 3, p.getPriceCents());
    sqlite3_bind_text(st, 4, p.getStatus().c_str(), -1, SQLITE_TRANSIENT);
    sqlite_check(sqlite3_step(st), cx_.raw(), "step insert product");
    sqlite3_finalize(st);
    p.setId(static_cast<long long>(sqlite3_last_insert_rowid(cx_.raw())));
    return p;
}

std::optional<Product> ProductRepositorySqlite::findById(long long id) {
    const char* sql = "SELECT id,name,description,price_cents,status FROM products WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(cx_.raw(), sql, -1, &st, nullptr), cx_.raw(), "prepare get product");
    sqlite3_bind_int64(st, 1, id);
    if (sqlite3_step(st) == SQLITE_ROW) {
        auto p = row_to_product(st);
        sqlite3_finalize(st);
        return p;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}

std::vector<Product> ProductRepositorySqlite::listAll() {
    const char* sql = "SELECT id,name,description,price_cents,status FROM products ORDER BY id DESC";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(cx_.raw(), sql, -1, &st, nullptr), cx_.raw(), "prepare list products");
    std::vector<Product> out;
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_product(st));
    sqlite3_finalize(st);
    return out;
}

bool ProductRepositorySqlite::update(const Product& p) {
    const char* sql = "UPDATE products SET name=?, description=?, price_cents=?, status=? WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(cx_.raw(), sql, -1, &st, nullptr), cx_.raw(), "prepare update product");
    sqlite3_bind_text(st, 1, p.getName().c_str(), -1, SQLITE_TRANSIENT);
    if (p.getDescription().has_value())
        sqlite3_bind_text(st, 2, p.getDescription()->c_str(), -1, SQLITE_TRANSIENT);
    else
        sqlite3_bind_null(st, 2);
    sqlite3_bind_int64(st, 3, p.getPriceCents());
    sqlite3_bind_text(st, 4, p.getStatus().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 5, p.getId());
    sqlite_check(sqlite3_step(st), cx_.raw(), "step update product");
    int changed = sqlite3_changes(cx_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

bool ProductRepositorySqlite::remove(long long id) {
    const char* sql = "DELETE FROM products WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(cx_.raw(), sql, -1, &st, nullptr), cx_.raw(), "prepare delete product");
    sqlite3_bind_int64(st, 1, id);
    sqlite_check(sqlite3_step(st), cx_.raw(), "step delete product");
    int changed = sqlite3_changes(cx_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}
