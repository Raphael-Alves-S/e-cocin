#include "OrderRepositorySqlite.h"
#include "Helpers.h"
#include <chrono>

static Order row_to_order(sqlite3_stmt* s) {
    Order o;
    o.setId(static_cast<long long>(sqlite3_column_int64(s, 0)));
    o.setClientId(static_cast<long long>(sqlite3_column_int64(s, 1)));
    o.setProductId(static_cast<long long>(sqlite3_column_int64(s, 2)));
    o.setShippingAddressId(static_cast<long long>(sqlite3_column_int64(s, 3)));
    o.setQuantity(static_cast<int>(sqlite3_column_int(s, 4)));
    o.setUnitPrice(sqlite3_column_double(s, 5));
    // total_price vem do banco; mas garantimos consistência quando salvar/atualizar
    double total = sqlite3_column_double(s, 6);
    // Se quiser recalcular: o.calculateTotal();
    // mas como queremos refletir o valor persistido:
    // (não há setter público para total; ele é calculado em setQuantity/setUnitPrice)
    // então assumimos que total_price no banco está consistente com quantity*unit_price.
    o.setStatus(reinterpret_cast<const char*>(sqlite3_column_text(s, 7)));

    long long secs = sqlite3_column_int64(s, 8);
    o.setCreateDate(ecocin::core::Timestamp{ std::chrono::system_clock::time_point{ std::chrono::seconds{secs} } });

    // Como não há setter direto de totalPrice, forçamos via utilitário:
    // atualiza internamente ao ajustar qty/price.
    // (Se setQuantity/setUnitPrice já chamam calculateTotal, estamos ok.)
    // Reaplica para manter coerência do objeto em memória:
    o.setQuantity(o.getQuantity());
    o.setUnitPrice(o.getUnitPrice());

    return o;
}

namespace ecocin::infra::repositories::sqlite {

// Persiste um novo pedido no banco de dados.
// O método recebe um objeto de domínio 'Order', calcula o valor total para garantir consistência,
// define a data de criação e o insere na tabela 'orders'.
// Este encapsulamento da lógica de criação assegura que todo pedido salvo seja válido e completo.
Order OrderRepositorySqlite::create(const Order& in) {
    Order o = in;

    // Garante total antes de persistir
    o.calculateTotal();
    const auto now   = std::chrono::system_clock::now();
    const auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    o.setCreateDate(ecocin::core::Timestamp{ now });

    const char* sql =
        "INSERT INTO orders("
        " client_id, product_id, shipping_address_id, quantity, unit_price, total_price, status, create_date"
        ") VALUES (?,?,?,?,?,?,?,?)";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare insert order");

    sqlite3_bind_int64(st, 1, o.getClientId());
    sqlite3_bind_int64(st, 2, o.getProductId());
    sqlite3_bind_int64(st, 3, o.getShippingAddressId());
    sqlite3_bind_int(st,   4, o.getQuantity());
    sqlite3_bind_double(st,5, o.getUnitPrice());
    sqlite3_bind_double(st,6, o.getUnitPrice() * o.getQuantity()); // total_price
    sqlite3_bind_text(st,  7, o.getStatus().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 8, static_cast<sqlite3_int64>(epoch));

    sqlite_check(sqlite3_step(st), connection_.raw(), "step insert order");
    sqlite3_finalize(st);

    o.setId(static_cast<long long>(sqlite3_last_insert_rowid(connection_.raw())));
    return o;
}

// Busca um pedido específico pelo seu ID.
// A responsabilidade de executar a consulta e transformar o resultado em um objeto 'Order'
// está contida neste método, seguindo o princípio de responsabilidade única.
// O uso de `std::optional` comunica de forma clara a possibilidade de o pedido não ser encontrado.
std::optional<Order> OrderRepositorySqlite::findById(long long id) {
    const char* sql =
        "SELECT id,client_id,product_id,shipping_address_id,quantity,unit_price,total_price,status,create_date "
        "FROM orders WHERE id=?";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare get order by id");

    sqlite3_bind_int64(st, 1, id);

    if (sqlite3_step(st) == SQLITE_ROW) {
        auto o = row_to_order(st);
        sqlite3_finalize(st);
        return o;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}

// Retorna uma lista de todos os pedidos existentes no sistema.
// Este método abstrai a complexidade de consultar e mapear múltiplos registros do banco de dados,
// fornecendo uma interface simples para a camada de serviço.
std::vector<Order> OrderRepositorySqlite::listAll() {
    const char* sql =
        "SELECT id,client_id,product_id,shipping_address_id,quantity,unit_price,total_price,status,create_date "
        "FROM orders ORDER BY id DESC";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare list orders");

    std::vector<Order> out;
    while (sqlite3_step(st) == SQLITE_ROW) {
        out.emplace_back(row_to_order(st));
    }
    sqlite3_finalize(st);
    return out;
}

// Atualiza os dados de um pedido existente.
// O método garante que o preço total seja recalculado antes de persistir as alterações,
// mantendo a integridade dos dados. A lógica de atualização fica isolada nesta camada,
// o que facilita a manutenção e evita duplicação de código.
bool OrderRepositorySqlite::update(const Order& oIn) {
    Order o = oIn;
    o.calculateTotal();

    const char* sql =
        "UPDATE orders "
        "SET client_id=?, product_id=?, shipping_address_id=?, quantity=?, "
        "    unit_price=?, total_price=?, status=? "
        "WHERE id=?";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare update order");

    sqlite3_bind_int64(st, 1, o.getClientId());
    sqlite3_bind_int64(st, 2, o.getProductId());
    sqlite3_bind_int64(st, 3, o.getShippingAddressId());
    sqlite3_bind_int(st,   4, o.getQuantity());
    sqlite3_bind_double(st,5, o.getUnitPrice());
    sqlite3_bind_double(st,6, o.getUnitPrice() * o.getQuantity());
    sqlite3_bind_text(st,  7, o.getStatus().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 8, o.getId());

    sqlite_check(sqlite3_step(st), connection_.raw(), "step update order");
    const int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

// Remove um pedido do banco de dados com base no seu ID.
// A operação de exclusão é encapsulada, de modo que a camada de serviço
// não precisa se preocupar com a sintaxe SQL ou o tratamento de conexões.
bool OrderRepositorySqlite::remove(long long id) {
    const char* sql = "DELETE FROM orders WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare delete order");
    sqlite3_bind_int64(st, 1, id);
    sqlite_check(sqlite3_step(st), connection_.raw(), "step delete order");
    const int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

// Lista todos os pedidos associados a um determinado cliente.
// Este é um exemplo de método de consulta específico do negócio, que abstrai
// uma necessidade comum da aplicação em uma chamada de método simples e clara.
std::vector<Order> OrderRepositorySqlite::listByClientId(long long clientId) {
    const char* sql =
        "SELECT id,client_id,product_id,shipping_address_id,quantity,unit_price,total_price,status,create_date "
        "FROM orders WHERE client_id=? "
        "ORDER BY create_date DESC, id DESC";

    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare list orders by client_id");

    sqlite3_bind_int64(st, 1, clientId);

    std::vector<Order> out;
    while (sqlite3_step(st) == SQLITE_ROW) {
        out.emplace_back(row_to_order(st));
    }
    sqlite3_finalize(st);
    return out;
}

// Atualiza apenas o status de um pedido específico.
// Em vez de carregar e salvar o objeto 'Order' inteiro, este método realiza uma
// operação mais performática e focada, demonstrando uma otimização comum em repositórios.
bool OrderRepositorySqlite::updateStatus(long long id, const std::string& newStatus) {
    const char* sql = "UPDATE orders SET status=? WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare update order status");

    sqlite3_bind_text(st, 1, newStatus.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 2, id);

    sqlite_check(sqlite3_step(st), connection_.raw(), "step update order status");
    const int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

// Altera o endereço de entrega de um pedido.
// Assim como `updateStatus`, este método encapsula uma atualização parcial e específica,
// o que melhora a eficiência e a clareza da intenção do código.
bool OrderRepositorySqlite::updateShippingAddress(long long id, long long newAddressId) {
    const char* sql = "UPDATE orders SET shipping_address_id=? WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr),
                 connection_.raw(), "prepare update order address");

    sqlite3_bind_int64(st, 1, newAddressId);
    sqlite3_bind_int64(st, 2, id);

    sqlite_check(sqlite3_step(st), connection_.raw(), "step update order address");
    const int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

} // namespace ecocin::infra::repositories::sqlite
