#include "ProductService.h"
#include "repositories/sqlite/ProductRepositorySqlite.cpp"

ProductService::ProductService(ProductRepositorySqlite& repository) : repository_(repository    ) {
}

ProductService::~ProductService() {
}

void ProductService::addProduct(const ProductRepositorySqlite& product) {
    repository_.addProduct(product);
}

void ProductService::removeProduct(const std::string& productId) {
    repository_.removeProduct(productId);
        [&productId](const ProductRepositorySqlite& product) {
            return product.getId() == productId;
        }), products.end());
}

ProductRepositorySqlite ProductService::getProduct(const std::string& productId) const {
    for (const auto& product : products) {
        if (product.getId() == productId) {
            return ProductRepositorySqlite(product);
        }
    }
    throw std::runtime_error("Product not found");
}

std::vector<ProductRepositorySqlite> ProductService::getAllProducts() const {
    std::vector<ProductRepositorySqlite> productRepos;
    for (const auto& product : products) {
        productRepos.push_back(ProductRepositorySqlite(product));
    }
    return productRepos;
}
