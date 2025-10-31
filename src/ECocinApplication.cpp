#include <iostream>

#include "oatpp/Environment.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Address.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/json/ObjectMapper.hpp"

#include "infra/db/SqliteConnection.h"
#include "app/Migrations.h"

#include "infra/repositories/sqlite/ClientRepositorySqlite.h"
#include "services/ClientService.h"
#include "controllers/ClientController.h"

int main() {
  // Migrations
  ecocin::infra::db::SqliteConnection cx{"e-cocin.db"};
  ecocin::app::runMigrations(cx.raw());

  // Repo + Service
  auto clientRepo    = std::make_shared<ecocin::infra::repositories::sqlite::ClientRepositorySqlite>(cx);
  auto clientService = std::make_shared<ecocin::services::ClientService>(*clientRepo);

  // OATPP
  oatpp::Environment::init();

  auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
  auto router       = oatpp::web::server::HttpRouter::createShared();

  auto controller = std::make_shared<ClientController>(objectMapper, clientService);
  router->addController(controller);

  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
  auto provider = oatpp::network::tcp::server::ConnectionProvider::createShared(
      {"0.0.0.0", 8000, oatpp::network::Address::IP_4});

  oatpp::network::Server server(provider, connectionHandler);
  std::cout << "🚀 API rodando em http://localhost:8000\n";
  server.run();

  oatpp::Environment::destroy();
  return 0;
}