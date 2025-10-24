#include "OrderItem.h"

OrderItem::OrderItem()
    : id_(0), orderId_(0), productId_(0), quantity_(0), unitPrice_(0.0) {}

OrderItem::OrderItem(long long productId, int quantity, double unitPrice)
    : id_(0),
      orderId_(0), 
      productId_(productId),
      quantity_(quantity),
      unitPrice_(unitPrice) {}