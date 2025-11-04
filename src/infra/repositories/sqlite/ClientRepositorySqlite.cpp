#include "ClientRepositorySqlite.h"
#include "Helpers.h"
#include <chrono>


static Client row_to_client(sqlite3_stmt* s) {
    Client client;
    client.setId(static_cast<long long>(sqlite3_column_int64(s, 0)));
    client.setName(reinterpret_cast<const char*>(sqlite3_column_text(s, 1)));
    client.setEmail(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
    client.setCpf(reinterpret_cast<const char*>(sqlite3_column_text(s, 3)));
    long long secs = sqlite3_column_int64(s, 4);
    client.setCreateDate(std::chrono::system_clock::time_point{std::chrono::seconds{secs}});
    return client;
}

// Este método é responsável por adicionar um novo cliente ao banco de dados.
// Ele recebe um objeto 'Client', define a data de criação, executa a inserção SQL
// e, em seguida, atualiza o objeto com o ID gerado, retornando-o.
// A abstração do acesso a dados permite que o resto da aplicação
// manipule objetos 'Client' sem conhecer os detalhes do SQL.
Client ecocin::infra::repositories::sqlite::ClientRepositorySqlite::create(const Client& in) {
    Client client = in;

    // agora
    auto now   = std::chrono::system_clock::now();
    auto epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    client.setCreateDate(now);

    // use a MESMA coluna da migration (create_date)
    const char* sql = "INSERT INTO clients(name,email,cpf,create_date) VALUES(?,?,?,?)";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare insert client");
    sqlite3_bind_text(st, 1, client.getName().c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, client.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, client.getCpf().c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 4, epoch);
    sqlite_check(sqlite3_step(st), connection_.raw(), "step insert client");
    sqlite3_finalize(st);

    client.setId(static_cast<long long>(sqlite3_last_insert_rowid(connection_.raw())));
    return client;
}

// Realiza a busca de um cliente pelo seu ID único.
// O método encapsula a consulta SQL e a lógica de mapeamento do resultado para um objeto 'Client'.
// O uso de `std::optional` é uma boa prática que torna explícito que o cliente
// pode não existir, evitando o uso de ponteiros nulos ou exceções para controle de fluxo.
std::optional<Client> ecocin::infra::repositories::sqlite::ClientRepositorySqlite::findById(long long id) {
    const char* sql = "SELECT id,name,email,cpf,create_date FROM clients WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare get client");
    sqlite3_bind_int64(st, 1, id);
    if (sqlite3_step(st) == SQLITE_ROW) {
        auto c = row_to_client(st);
        sqlite3_finalize(st);
        return c;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}

// Busca um cliente utilizando o CPF, que é um identificador de negócio.
// Assim como o `findById`, este método isola a lógica de acesso a dados
// e utiliza `std::optional` para um retorno seguro e claro.
std::optional<Client> ecocin::infra::repositories::sqlite::ClientRepositorySqlite::findByCpf(const std::string& cpf) {
    const char* sql = "SELECT id,name,email,cpf,create_date FROM clients WHERE cpf=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare get client by cpf");
    sqlite3_bind_text(st, 1, cpf.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(st) == SQLITE_ROW) {
        auto c = row_to_client(st);
        sqlite3_finalize(st);
        return c;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}

// Retorna uma lista com todos os clientes cadastrados.
// A responsabilidade de consultar e montar a coleção de objetos 'Client'
// é totalmente delegada a este método, simplificando as camadas superiores da aplicação.
std::vector<Client> ecocin::infra::repositories::sqlite::ClientRepositorySqlite::listAll() {
    const char* sql = "SELECT id,name,email,cpf,create_date FROM clients ORDER BY id DESC";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare list clients");
    std::vector<Client> out;
    while (sqlite3_step(st) == SQLITE_ROW) {
        out.push_back(row_to_client(st));
    }
    sqlite3_finalize(st);
    return out;
}


// Atualiza as informações de um cliente existente no banco de dados.
// O método recebe um objeto 'Client' com os dados modificados e executa o comando UPDATE.
// O retorno booleano informa se a operação afetou alguma linha, indicando o sucesso da atualização.
// Isso demonstra o encapsulamento da lógica de modificação de dados.
bool ecocin::infra::repositories::sqlite::ClientRepositorySqlite::update(const Client& c) {
    const char* sql = "UPDATE clients SET name=?, email=?, cpf=? WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare update client");
    sqlite3_bind_text(st, 1, c.getName().c_str(),  -1, SQLITE_TRANSIENT); // getName()
    sqlite3_bind_text(st, 2, c.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, c.getCpf().c_str(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 4, c.getId());
    sqlite_check(sqlite3_step(st), connection_.raw(), "step update client");
    int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}

// Remove um cliente do sistema a partir do seu ID.
// A complexidade da operação de exclusão no banco de dados é completamente
// escondida da lógica de negócio, que apenas precisa invocar este método.
bool ecocin::infra::repositories::sqlite::ClientRepositorySqlite::remove(long long id) {
    const char* sql = "DELETE FROM clients WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare delete client");
    sqlite3_bind_int64(st, 1, id);
    sqlite_check(sqlite3_step(st), connection_.raw(), "step delete client");
    int changed = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changed > 0;
}
