#include "ProductRepositorySqlite.h"
#include "Helpers.h"
#include <chrono>

static Product row_to_product(sqlite3_stmt* s) {
    Product p;
    p.setId(static_cast<long long>(sqlite3_column_int64(s, 0)));
    p.setName(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));

    if (sqlite3_column_type(s, 2) != SQLITE_NULL) {
        p.setDescription(std::string(reinterpret_cast<const char*>(sqlite3_column_text(s, 2))));
    } else {
        p.setDescription(std::string{});
    }


    p.setSku(ecocin::core::Uuid(reinterpret_cast<const char*>(sqlite3_column_text(s, 3))));


    p.setPrice(sqlite3_column_double(s, 4));

    p.setStockQuantity(static_cast<int>(sqlite3_column_int(s, 5)));
    p.setIsActive(sqlite3_column_int(s, 6) != 0);
    p.setCreateDate(std::chrono::system_clock::time_point{
        std::chrono::seconds{sqlite3_column_int64(s, 7)}
    });
    return p;
}

namespace ecocin::infra::repositories::sqlite {

// Insere um novo produto no banco de dados.
// O método recebe um objeto 'Product', define sua data de criação e o persiste.
// A lógica de conversão de tipos (como Uuid para string) e a montagem da instrução SQL
// são encapsuladas aqui, mantendo a camada de serviço limpa e focada na regra de negócio.
Product ProductRepositorySqlite::create(const Product& in) {
    Product p = in;
    const auto now   = std::chrono::system_clock::now();
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    p.setCreateDate(now);

    const char* sql =
        "INSERT INTO products(name, description, sku, price, stock_quantity, is_active, create_date) "
        "VALUES(?,?,?,?,?,?,?)";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare insert product");

    sqlite3_bind_text(st, 1, p.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, p.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    const std::string skuStr = p.getSku().str();
    sqlite3_bind_text(st, 3, skuStr.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(st, 4, p.getPrice());
    sqlite3_bind_int(st, 5, p.getStockQuantity());
    sqlite3_bind_int(st, 6, p.getIsActive() ? 1 : 0);
    sqlite3_bind_int64(st, 7, static_cast<sqlite3_int64>(epoch));

    sqlite_check(sqlite3_step(st), connection_.raw(), "step insert product");
    sqlite3_finalize(st);

    p.setId(static_cast<long long>(sqlite3_last_insert_rowid(connection_.raw())));
    return p;
}

// Busca um produto pelo seu ID.
// Este método demonstra a abstração do acesso a dados, escondendo a complexidade
// da consulta SQL e do mapeamento de colunas para os atributos do objeto 'Product'.
// O retorno `std::optional` gerencia de forma elegante o caso em que o produto não é encontrado.
std::optional<Product> ProductRepositorySqlite::findById(long long id) {
    const char* sql =
        "SELECT id,name,description,sku,price,stock_quantity AS stock,is_active,create_date "
        "FROM products WHERE id=?";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare get product by id");
    sqlite3_bind_int64(st, 1, id);

    if (sqlite3_step(st) == SQLITE_ROW) {
        auto p = row_to_product(st);
        sqlite3_finalize(st);
        return p;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}

// Localiza um produto pelo seu SKU (Stock Keeping Unit), um identificador único de negócio.
// Fornecer métodos de busca por diferentes chaves de negócio é uma prática comum
// em repositórios para dar flexibilidade à camada de serviço.
std::optional<Product> ProductRepositorySqlite::findBySku(const std::string& sku) {
    const char* sql =
        "SELECT id,name,description,sku,price,stock_quantity AS stock,is_active,create_date "
        "FROM products WHERE sku=?";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare get product by sku");
    sqlite3_bind_text(st, 1, sku.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(st) == SQLITE_ROW) {
        auto p = row_to_product(st);
        sqlite3_finalize(st);
        return p;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}

// Retorna uma lista com todos os produtos cadastrados.
// O método encapsula a iteração sobre o resultado da consulta e a construção
// da coleção de objetos 'Product', simplificando o código que o consome.
std::vector<Product> ProductRepositorySqlite::listAll() {
    const char* sql =
        "SELECT id,name,description,sku,price,stock_quantity AS stock,is_active,create_date "
        "FROM products ORDER BY id DESC";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare list products");

    std::vector<Product> out;
    while (sqlite3_step(st) == SQLITE_ROW) {
        out.push_back(row_to_product(st));
    }
    sqlite3_finalize(st);
    return out;
}

// Atualiza as informações de um produto existente.
// A responsabilidade de mapear os atributos do objeto 'Product' para os parâmetros
// da instrução SQL UPDATE está totalmente contida neste método.
// O retorno booleano fornece um feedback claro sobre o sucesso da operação.
bool ProductRepositorySqlite::update(const Product& p) {
    const char* sql =
        "UPDATE products SET name=?, description=?, sku=?, price=?, stock_quantity=?, is_active=? "
        "WHERE id=?";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare update product");

    sqlite3_bind_text(st, 1, p.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, p.getDescription().c_str(), -1, SQLITE_TRANSIENT);
    const std::string skuStr = p.getSku().str();
    sqlite3_bind_text(st, 3, skuStr.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(st, 4, p.getPrice());
    sqlite3_bind_int(st, 5, p.getStockQuantity());
    sqlite3_bind_int(st, 6, p.getIsActive() ? 1 : 0);
    sqlite3_bind_int64(st, 7, p.getId());

    sqlite_check(sqlite3_step(st), connection_.raw(), "step update product");
    const int changed = sqlite3_changes(connection_.raw());

    sqlite3_finalize(st);
    return changed > 0;
}

// Exclui um produto do banco de dados usando seu ID.
// Este método abstrai a operação de deleção, garantindo que a camada de serviço
// não precise lidar diretamente com o SQL, o que aumenta a segurança e a manutenibilidade.
bool ProductRepositorySqlite::remove(long long id) {
    const char* sql = "DELETE FROM products WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare delete product");
    sqlite3_bind_int64(st, 1, id);
    sqlite_check(sqlite3_step(st), connection_.raw(), "step delete product");
    const int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

}
