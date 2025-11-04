// Função para rodar migrações de banco de dados SQLite
// Cria tabelas e índices necessários se não existirem

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
  sku             TEXT    NOT NULL UNIQUE,   
  price           REAL    NOT NULL,       
  stock_quantity  INTEGER NOT NULL DEFAULT 0,
  is_active       INTEGER NOT NULL DEFAULT 1 CHECK (is_active IN (0,1)),
  create_date     INTEGER NOT NULL  
);

-- Índices auxiliares
CREATE INDEX IF NOT EXISTS idx_products_name       ON products(name);
CREATE INDEX IF NOT EXISTS idx_products_is_active  ON products(is_active);


-- ===========================
-- Tabela: addresses 
-- ===========================
CREATE TABLE IF NOT EXISTS addresses (  
  id              INTEGER PRIMARY KEY AUTOINCREMENT,
  client_id       INTEGER NOT NULL,
  street          TEXT    NOT NULL,
  number          TEXT    NOT NULL,
  city            TEXT    NOT NULL,
  state           TEXT    NOT NULL,
  zip             TEXT    NOT NULL,
  address_type    TEXT    NOT NULL,
  create_date     INTEGER NOT NULL,           


  FOREIGN KEY (client_id) REFERENCES clients(id)
);
-- Índices auxiliares
CREATE INDEX IF NOT EXISTS idx_addresses_client_id ON addresses(client_id);
CREATE INDEX IF NOT EXISTS idx_addresses_zip ON addresses(zip);

-- ===========================
-- Tabela: orders
-- ===========================
CREATE TABLE IF NOT EXISTS orders (
  id                   INTEGER PRIMARY KEY AUTOINCREMENT,
  client_id            INTEGER NOT NULL,
  product_id           INTEGER NOT NULL,
  shipping_address_id  INTEGER,
  quantity             INTEGER NOT NULL CHECK (quantity > 0),
  unit_price           REAL    NOT NULL CHECK (unit_price >= 0.0),
  total_price          REAL    NOT NULL CHECK (total_price >= 0.0),
  status               TEXT    NOT NULL DEFAULT 'PENDING',
  create_date          INTEGER NOT NULL,

  FOREIGN KEY (client_id)           REFERENCES clients(id),
  FOREIGN KEY (product_id)          REFERENCES products(id),
  FOREIGN KEY (shipping_address_id) REFERENCES addresses(id)
);

CREATE INDEX IF NOT EXISTS idx_orders_client_id   ON orders(client_id);
CREATE INDEX IF NOT EXISTS idx_orders_product_id  ON orders(product_id);
CREATE INDEX IF NOT EXISTS idx_orders_status      ON orders(status);
CREATE INDEX IF NOT EXISTS idx_orders_create_date ON orders(create_date);

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
