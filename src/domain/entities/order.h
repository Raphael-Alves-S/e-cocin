#ifndef ORDER_H
#define ORDER_H

#include <string>
#include "../core/Time.h"

class Order {
private:
    long long id_;
    long long clientId_;
    long long productId_;
    long long shippingAddressId_; // FK para addresses.id
    int quantity_;
    double unitPrice_;
    double totalPrice_;
    std::string status_;
    ecocin::core::Timestamp createDate_;

public:
    // Construtores
    Order();
    Order(long long clientId,
          long long productId,
          long long shippingAddressId,
          int quantity,
          double unitPrice,
          const std::string& status = "PENDING");

    // Getters
    long long getId() const { return id_; }
    long long getClientId() const { return clientId_; }
    long long getProductId() const { return productId_; }
    long long getShippingAddressId() const { return shippingAddressId_; }
    int getQuantity() const { return quantity_; }
    double getUnitPrice() const { return unitPrice_; }
    double getTotalPrice() const { return totalPrice_; }
    const std::string& getStatus() const { return status_; }
    ecocin::core::Timestamp getCreateDate() const { return createDate_; }

    // Setters
    void setId(long long id) { id_ = id; }
    void setClientId(long long clientId) { clientId_ = clientId; }
    void setProductId(long long productId) { productId_ = productId; }
    void setShippingAddressId(long long addrId) { shippingAddressId_ = addrId; }
    void setQuantity(int quantity);
    void setUnitPrice(double price);
    void setStatus(const std::string& status) { status_ = status; }
    void setCreateDate(ecocin::core::Timestamp date) { createDate_ = date; }

    // utilitário
    void calculateTotal();
};

#endif
