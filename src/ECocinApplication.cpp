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

#include "controllers/ProductController.h"
#include "infra/repositories/sqlite/ProductRepositorySqlite.h"
#include "services/ProductService.h"

#include "controllers/AddressController.h"
#include "infra/repositories/sqlite/AddressRepositorySqlite.h"
#include "services/AddressService.h"

#include "controllers/OrderController.h"
#include "infra/repositories/sqlite/OrderRepositorySqlite.h"
#include "services/OrderService.h"


int main() {
  // Migrations
  ecocin::infra::db::SqliteConnection cx{"e-cocin.db"};
  ecocin::app::runMigrations(cx.raw());

  // Client Repo + Service
  auto clientRepo    = std::make_shared<ecocin::infra::repositories::sqlite::ClientRepositorySqlite>(cx);
  auto clientService = std::make_shared<ecocin::services::ClientService>(*clientRepo);

  // Product Repo + Service
  auto productRepo    = std::make_shared<ecocin::infra::repositories::sqlite::ProductRepositorySqlite>(cx);
  auto productService = std::make_shared<ecocin::services::ProductService>(*productRepo);

  // Adress Repo + Service
  auto addressRepo    = std::make_shared<ecocin::infra::repositories::sqlite::AddressRepositorySqlite>(cx);
  auto addressService = std::make_shared<ecocin::services::AddressService>(*addressRepo, *clientRepo);

  // Order Repo + Service
  auto orderRepo    = std::make_shared<ecocin::infra::repositories::sqlite::OrderRepositorySqlite>(cx);
  auto orderService = std::make_shared<ecocin::services::OrderService>(
      *orderRepo, *clientRepo, *productRepo, *addressRepo);

  // OATPP
  oatpp::Environment::init();

  auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
  auto router       = oatpp::web::server::HttpRouter::createShared();

  auto controller = std::make_shared<ClientController>(objectMapper, clientService);
  router->addController(controller);

  auto productController = std::make_shared<ProductController>(objectMapper, productService);
  router->addController(productController);

  auto addressController = std::make_shared<AddressController>(objectMapper, addressService);
  router->addController(addressController);

  auto orderController = std::make_shared<OrderController>(objectMapper, orderService);
  router->addController(orderController);

  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
  auto provider = oatpp::network::tcp::server::ConnectionProvider::createShared(
      {"0.0.0.0", 8000, oatpp::network::Address::IP_4});

  oatpp::network::Server server(provider, connectionHandler);
  std::cout << "🚀 API rodando em http://localhost:8000\n";
  server.run();

  oatpp::Environment::destroy();
  return 0;
}