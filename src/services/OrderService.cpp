#include "OrderService.h"
#include <sstream>

using namespace ecocin;
using ecocin::services::OrderService;
using ecocin::services::OrderDetails;

// O construtor do serviço de pedidos utiliza injeção de dependência para todos os repositórios
// necessários. Este design promove baixo acoplamento e alta coesão, permitindo que o serviço
// orquestre operações complexas que envolvem múltiplas entidades do domínio (Pedidos, Clientes, etc.),
// sem se preocupar com a implementação do acesso a dados.
OrderService::OrderService(
  infra::repositories::sqlite::OrderRepositorySqlite& orderRepo,
  infra::repositories::sqlite::ClientRepositorySqlite& clientRepo,
  infra::repositories::sqlite::ProductRepositorySqlite& productRepo,
  infra::repositories::sqlite::AddressRepositorySqlite& addressRepo)
  : orderRepo_(orderRepo)
  , clientRepo_(clientRepo)
  , productRepo_(productRepo)
  , addressRepo_(addressRepo) {}

// Resolve o endereço de entrega para um cliente com base em um tipo preferencial.
// A lógica de negócio implementada aqui é flexível: primeiro, busca um endereço que
// corresponda exatamente ao tipo solicitado. Se não encontrar, e o cliente tiver apenas
// um endereço cadastrado, ele é usado como padrão. Isso simplifica a experiência do usuário.
std::optional<Address> OrderService::resolveAddressForClient(long long clientId,
                                                             const std::string& type) {
  // 1) tenta por tipo
  auto list = addressRepo_.listByClientId(clientId);
  Address only{};
  bool hasOnly = false;

  for (const auto& a : list) {
    if (a.getAddressType() == type) {
      return a;
    }
  }
  // 2) fallback: se tiver exatamente 1 endereço, usa-o
  if (list.size() == 1) {
    return list.front();
  }
  // 3) nenhum adequado
  return std::nullopt;
}

// Orquestra a criação de um novo pedido a partir de informações de negócio (CPF, SKU).
// Este método é um excelente exemplo da responsabilidade da camada de serviço: ele coordena
// múltiplos repositórios para validar o cliente, o produto e o endereço, para então
// montar e persistir o objeto de domínio 'Order'. Toda a complexidade é abstraída
// em uma única chamada de método.
std::optional<Order> OrderService::createByCpfSkuAndType(const std::string& cpf,
                                                         const std::string& sku,
                                                         const std::string& shippingAddressType,
                                                         int quantity) {
  if (cpf.empty() || sku.empty() || quantity <= 0) return std::nullopt;

  auto clientOpt = clientRepo_.findByCpf(cpf);
  if (!clientOpt) return std::nullopt;
  const long long clientId = clientOpt->getId();

  auto productOpt = productRepo_.findBySku(sku);
  if (!productOpt) return std::nullopt;
  const long long productId = productOpt->getId();
  const double unitPrice = productOpt->getPrice(); // vem do produto

  auto addrOpt = resolveAddressForClient(clientId, shippingAddressType);
  if (!addrOpt) {
    // sem endereço compatível — se quiser, retorne std::nullopt e o controller manda 400 com msg clara
    return std::nullopt;
  }

  Order o(clientId, productId, addrOpt->getId(), quantity, unitPrice, "PENDING");
  o.calculateTotal();
  return orderRepo_.create(o);
}

// Busca um pedido pelo seu ID.
// É um método simples que delega a chamada ao repositório, mas sua existência na camada
// de serviço é crucial para manter a consistência da arquitetura e o isolamento das camadas.
std::optional<Order> OrderService::getById(long long id) {
  return orderRepo_.findById(id);
}

// Lista os detalhes de todos os pedidos de um cliente, buscando-o pelo CPF.
// Este método vai além de uma simples listagem: ele enriquece os dados do pedido
// com informações completas do cliente, produto e endereço, montando uma estrutura
// `OrderDetails`. Isso demonstra como a camada de serviço pode agregar valor
// ao combinar e transformar dados de diferentes fontes para atender a uma necessidade específica da aplicação.
std::vector<OrderDetails> OrderService::listDetailsByCpf(const std::string& cpf) {
  std::vector<OrderDetails> out;
  auto clientOpt = clientRepo_.findByCpf(cpf);
  if (!clientOpt) return out;

  const long long clientId = clientOpt->getId();
  auto orders = orderRepo_.listByClientId(clientId);
  if (orders.empty()) return out;

  // carrega uma vez o cliente
  const Client client = *clientOpt;

  // para cada order, busca product e address
  for (const auto& o : orders) {
    auto productOpt = productRepo_.findById(o.getProductId());
    auto addressOpt = addressRepo_.findById(o.getShippingAddressId());
    if (!productOpt || !addressOpt) continue;

    out.push_back(OrderDetails{ o, client, *productOpt, *addressOpt });
  }
  return out;
}
