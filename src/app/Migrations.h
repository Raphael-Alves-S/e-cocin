#ifndef ECOCIN_APP_MIGRATIONS_H
#define ECOCIN_APP_MIGRATIONS_H

#include <sqlite3.h>
#include <stdexcept>
#include <string>

namespace ecocin::app {

// SQL idempotente (CREATE IF NOT EXISTS) — rode no boot
static const char* MIGRATION_SQL = R"SQL(
PRAGMA foreign_keys = ON;

-- ===========================
-- Tabela: clients
-- ===========================
CREATE TABLE IF NOT EXISTS clients (
  id           INTEGER PRIMARY KEY AUTOINCREMENT,
  name         TEXT    NOT NULL,
  email        TEXT    NOT NULL UNIQUE,
  cpf          TEXT    NOT NULL UNIQUE,
  create_date  INTEGER NOT NULL         -- epoch seconds
);

-- Índices auxiliares (opcionais para busca)
CREATE INDEX IF NOT EXISTS idx_clients_name  ON clients(name);
CREATE INDEX IF NOT EXISTS idx_clients_email ON clients(email);

-- ===========================
-- Tabela: products
-- ===========================
CREATE TABLE IF NOT EXISTS products (
  id              INTEGER PRIMARY KEY AUTOINCREMENT,
  name            TEXT    NOT NULL,
  description     TEXT,
  sku             TEXT    NOT NULL UNIQUE,   -- UUID como string
  price           REAL    NOT NULL,          -- atenção a arredondamento; considerar INTEGER (centavos)
  stock_quantity  INTEGER NOT NULL DEFAULT 0,
  is_active       INTEGER NOT NULL DEFAULT 1 CHECK (is_active IN (0,1)),
  create_date     INTEGER NOT NULL           -- epoch seconds
);

-- Índices auxiliares
CREATE INDEX IF NOT EXISTS idx_products_name       ON products(name);
CREATE INDEX IF NOT EXISTS idx_products_is_active  ON products(is_active);

)SQL";

// Executa o SQL acima (lança std::runtime_error em caso de falha)
inline void runMigrations(sqlite3* db) {
  char* err = nullptr;
  if (sqlite3_exec(db, MIGRATION_SQL, nullptr, nullptr, &err) != SQLITE_OK) {
    std::string msg = err ? err : "unknown";
    if (err) sqlite3_free(err);
    throw std::runtime_error("migration failed: " + msg);
  }
}

} // namespace ecocin::app

#endif // ECOCIN_APP_MIGRATIONS_H
