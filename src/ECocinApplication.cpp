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


// A função `main` é o ponto de entrada da aplicação. Ela é responsável por
// inicializar todos os componentes da arquitetura e iniciar o servidor web.
// Este processo é conhecido como "Composição da Raiz" (Composition Root),
// um padrão onde todas as dependências são construídas e injetadas em um único local.
int main() {
  // O primeiro passo é configurar o banco de dados.
  // Uma conexão com o SQLite é estabelecida e as migrações (criação/atualização de tabelas)
  // são executadas para garantir que o esquema do banco esteja atualizado.
  ecocin::infra::db::SqliteConnection cx{"e-cocin.db"};
  ecocin::app::runMigrations(cx.raw());

  // Aqui começa a injeção de dependência manual.
  // Para cada entidade (Cliente, Produto, etc.), o padrão é o mesmo:
  // 1. Cria-se uma instância do Repositório, passando a conexão com o banco.
  // 2. Cria-se uma instância do Serviço, passando o repositório como dependência.
  // Este processo constrói a cadeia de dependências de baixo para cima (dados -> negócio).
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

  // Com os serviços prontos, a próxima etapa é configurar a camada web usando o framework OATPP.
  oatpp::Environment::init();

  // O ObjectMapper é responsável por converter objetos C++ para JSON e vice-versa.
  // O HttpRouter gerencia o mapeamento das rotas (ex: "/clients") para os métodos dos controllers.
  auto objectMapper = std::make_shared<oatpp::json::ObjectMapper>();
  auto router       = oatpp::web::server::HttpRouter::createShared();

  // Agora, a injeção de dependência continua na camada de apresentação:
  // 1. Cria-se uma instância de cada Controller.
  // 2. O Controller recebe o ObjectMapper (para manipulação de JSON) e o Serviço correspondente.
  // 3. O Controller é registrado no roteador, associando seus endpoints (ex: GET /clients)
  //    às funções que irão tratar as requisições.
  auto controller = std::make_shared<ClientController>(objectMapper, clientService);
  router->addController(controller);

  auto productController = std::make_shared<ProductController>(objectMapper, productService);
  router->addController(productController);

  auto addressController = std::make_shared<AddressController>(objectMapper, addressService);
  router->addController(addressController);

  auto orderController = std::make_shared<OrderController>(objectMapper, orderService);
  router->addController(orderController);

  // Com todas as rotas e controllers configurados no roteador,
  // os componentes finais do servidor são montados.
  auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
  auto provider = oatpp::network::tcp::server::ConnectionProvider::createShared(
      {"0.0.0.0", 8000, oatpp::network::Address::IP_4}); // Escuta em todas as interfaces na porta 8000.

  // O objeto 'Server' é criado, unindo o provedor de conexão (que aceita conexões TCP)
  // com o manipulador de conexões (que processa as requisições HTTP através do roteador).
  oatpp::network::Server server(provider, connectionHandler);
  std::cout << "🚀 API rodando em http://localhost:8000\n";

  // O método 'run()' inicia o loop do servidor, que fica aguardando e processando requisições.
  // Este é um processo bloqueante que mantém a aplicação viva.
  server.run();

  // Após o término do servidor (ex: com um sinal de interrupção),
  // o ambiente do OATPP é finalizado para liberar recursos.
  oatpp::Environment::destroy();
  return 0;
}
