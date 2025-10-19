#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <iostream>

int main() {
  // só para provar que as libs linkam
  nlohmann::json j = { {"hello", "world"}, {"answer", 42} };
  spdlog::info("JSON: {}", j.dump());

  // abertura de SQLite (não precisa criar nada)
  sqlite3* db = nullptr;
  if (sqlite3_open(":memory:", &db) == SQLITE_OK) {
    spdlog::info("SQLite in-memory OKKKK");
    sqlite3_close(db);
  } else {
    spdlog::error("Falha ao abrir SQLite");
  }

  std::cout << "e-cocin build OK\n";
  return 0;
}
