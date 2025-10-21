#include "infra/db/SqliteConnection.h"
#include "app/Migrations.h"

int main() {
  ecocin::infra::db::SqliteConnection cx{"e-cocin.db"};
  ecocin::app::runMigrations(cx.raw());
  // ... instancie seus repositórios e siga o fluxo
}
