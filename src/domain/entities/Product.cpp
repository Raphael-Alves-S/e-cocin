#include "Product.h"
#include <chrono>

// Construtores
Product::Product()
    : createDate_(std::chrono::system_clock::now()) {}
Product::Product(const std::string& name,
                 const std::string& description,
                 const ecocin::core::Uuid& sku,
                 double price,
                 int stockQuantity,
                 bool isActive)
    : name_(name),
      description_(description),
      sku_(sku),
      price_(price),
      stockQuantity_(stockQuantity),
      isActive_(isActive),
      createDate_(std::chrono::system_clock::now()) {}  