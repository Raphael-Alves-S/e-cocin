#ifndef ECOCIN_SERVICES_PRODUCTSERVICE_H
#define ECOCIN_SERVICES_PRODUCTSERVICE_H

#include <string>
#include <vector>
#include "entities/Product.h"

class ProductService {
public:
    ProductService();
    ~ProductService();

    void addProduct(const Product& product);
    void removeProduct(const std::string& productId);
    Product getProduct(const std::string& productId) const;
    std::vector<Product> getAllProducts() const;

private:
    std::vector<Product> products;
};

#endif // ECOCIN_SERVICES_PRODUCTSERVICE_H