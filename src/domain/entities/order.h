#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <vector>
#include "core/Time.h" 
#include "domain/entities/OrderItem.h"   

class Order {
private:
    long long id_;
    long long clientId_; 
    ecocin::core::Timestamp createDate_;
    std::string status_; 
    std::vector<OrderItem> items_;
    double totalPrice_; 

public:
    Order();
    Order(long long clientId, const std::string& status = "PENDING");

    long long getId() const { return id_; }
    long long getClientId() const { return clientId_; }
    ecocin::core::Timestamp getCreateDate() const { return createDate_; }
    const std::string& getStatus() const { return status_; }
    const std::vector<OrderItem>& getItems() const { return items_; }
    double getTotalPrice() const { return totalPrice_; }

    void setId(long long id) { id_ = id; }
    void setClientId(long long clientId) { clientId_ = clientId; }
    void setCreateDate(ecocin::core::Timestamp createDate) { createDate_ = createDate; }
    void setStatus(const std::string& status) { status_ = status; }
    
    void setItems(const std::vector<OrderItem>& items);
    void addItem(const OrderItem& item);
    void calculateTotalPrice();
};

#endif 