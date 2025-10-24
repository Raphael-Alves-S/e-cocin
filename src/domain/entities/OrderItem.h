#ifndef ORDERITEM_H
#define ORDERITEM_H

#include <string>

class OrderItem {
private:
    long long id_; // Identificador único do item do pedido
    long long orderId_;   // Identificador do pedido ao qual o item pertence
    long long productId_; // Identificador do produto
    int quantity_; // Quantidade do produto no item do pedido
    double unitPrice_; // Preço unitário do produto no item do pedido

public:
    // Construtores
    OrderItem();
    OrderItem(long long productId, int quantity, double unitPrice); 

    // Getters
    long long getId() const { return id_; }
    long long getOrderId() const { return orderId_; }
    long long getProductId() const { return productId_; }
    int getQuantity() const { return quantity_; }
    double getUnitPrice() const { return unitPrice_; }

    // Setters
    void setId(long long id) { id_ = id; }
    void setOrderId(long long orderId) { orderId_ = orderId; }
    void setProductId(long long productId) { productId_ = productId; }
    void setQuantity(int quantity) { quantity_ = quantity; }
    void setUnitPrice(double price) { unitPrice_ = price; }
};

#endif 