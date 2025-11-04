#include "Order.h"
#include <chrono>
#include <numeric>

Order::Order()
    : id_(0),
      clientId_(0),
      productId_(0),
      shippingAddressId_(0),
      quantity_(1),
      unitPrice_(0.0),
      totalPrice_(0.0),
      status_("PENDING"),
      createDate_(std::chrono::system_clock::now()) {}

Order::Order(long long clientId,
             long long productId,
             long long shippingAddressId,
             int quantity,
             double unitPrice,
             const std::string& status)
    : id_(0),
      clientId_(clientId),
      productId_(productId),
      shippingAddressId_(shippingAddressId),
      quantity_(quantity),
      unitPrice_(unitPrice),
      totalPrice_(quantity * unitPrice),
      status_(status),
      createDate_(std::chrono::system_clock::now()) {}

void Order::setQuantity(int quantity) {
    quantity_ = quantity;
    calculateTotal();
}

void Order::setUnitPrice(double price) {
    unitPrice_ = price;
    calculateTotal();
}

void Order::calculateTotal() {
    totalPrice_ = static_cast<double>(quantity_) * unitPrice_;
}