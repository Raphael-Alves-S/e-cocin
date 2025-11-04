#ifndef ECOCIN_INFRA_DB_SQLITECONNECTION_H
#define ECOCIN_INFRA_DB_SQLITECONNECTION_H

#include <sqlite3.h>
#include <string>
#include <stdexcept>

namespace ecocin::infra::db {

// Conexão com banco de dados SQLite
class SqliteConnection {
private:
    sqlite3* db_{nullptr}; // Ponteiro para a conexão SQLite

public:
    explicit SqliteConnection(const std::string& path) {
        if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) { // Abre a conexão com o banco de dados
            throw std::runtime_error(std::string("SQLite open failed: ") + sqlite3_errmsg(db_)); // Lança exceção em caso de falha
        }
        char* err = nullptr;
        sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &err); // Habilita chaves estrangeiras
        if (err) { std::string e = err; sqlite3_free(err); throw std::runtime_error(e); }
    }

    ~SqliteConnection() { if (db_) sqlite3_close(db_); } // Fecha a conexão ao destruir o objeto

    SqliteConnection(const SqliteConnection&) = delete;
    SqliteConnection& operator=(const SqliteConnection&) = delete;

    sqlite3* raw() const { return db_; }
};

} // namespace ecocin::infra::db

#endif // ECOCIN_INFRA_DB_SQLITECONNECTION_H
