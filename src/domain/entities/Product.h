#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include "core/Time.h"
#include "core/Uuid.h"

class Product {
private:
    long long id_;
    std::string name_;
    std::string description_;
    ecocin::core::Uuid sku_;
    double price_;
    int stockQuantity_;
    bool isActive_;
    ecocin::core::Timestamp createDate_;

public:
    // Construtores
    Product();
    Product(const std::string& name,
            const std::string& description,
            const ecocin::core::Uuid& sku,
            double price,
            int stockQuantity,
            bool isActive);
    // Getters
    long long           getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    const ecocin::core::Uuid& getSku() const { return sku_; }
    double getPrice() const { return price_; }
    int getStockQuantity() const { return stockQuantity_; }
    bool getIsActive() const { return isActive_; }
    ecocin::core::Timestamp getCreateDate() const { return createDate_; }
    // Setters
    void setId(long long id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setSku(const ecocin::core::Uuid& sku) { sku_ = sku; }
    void setPrice(double price) { price_ = price; }
    void setStockQuantity(int stockQuantity) { stockQuantity_ = stockQuantity; }
    void setIsActive(bool isActive) { isActive_ = isActive; }
    void setCreateDate(ecocin::core::Timestamp createDate) { createDate_ = createDate; }

};
#endif 