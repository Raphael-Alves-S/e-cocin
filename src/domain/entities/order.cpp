#include "Order.h"
#include <chrono>  
#include <numeric>  

Order::Order()
    : id_(0),
      clientId_(0),
      createDate_(std::chrono::system_clock::now()), 
      status_("PENDING"),
      totalPrice_(0.0) {}

Order::Order(long long clientId, const std::string& status)
    : id_(0),
      clientId_(clientId),
      createDate_(std::chrono::system_clock::now()), 
      status_(status),
      totalPrice_(0.0) {}

void Order::setItems(const std::vector<OrderItem>& items) {
    items_ = items;
    calculateTotalPrice(); 
}

void Order::addItem(const OrderItem& item) {
    items_.push_back(item);
    calculateTotalPrice();
}

void Order::calculateTotalPrice() {
    totalPrice_ = std::accumulate(items_.begin(), items_.end(), 0.0,
        [](double sum, const OrderItem& item) {
            return sum + (item.getUnitPrice() * item.getQuantity());
        });
}