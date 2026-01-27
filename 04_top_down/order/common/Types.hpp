#pragma once
#include <cstdint>
#include "Constants.hpp"

namespace orderbook {

// Timestamp in nanoseconds since epoch
using Timestamp = uint64_t;

// Price represented as fixed-point (price * 10000 for 4 decimal places)
// e.g., 123.4567 -> 1234567
using Price = int64_t;
using Quantity = uint64_t;
using OrderId = uint64_t;
using SequenceNumber = uint64_t;

enum class Side : uint8_t {
    Buy = 0,
    Sell = 1
};

enum class MessageType : uint8_t {
    Add = 'A',
    Modify = 'M',
    Delete = 'D',
    Trade = 'T',
    Snapshot = 'S'
};

// Cache-line aligned order structure for optimal memory access
struct alignas(CACHE_LINE_SIZE) Order {
    OrderId id;
    Price price;
    Quantity quantity;
    Side side;
    Timestamp timestamp;

    Order() = default;
    Order(OrderId id_, Price price_, Quantity qty_, Side side_, Timestamp ts_)
        : id(id_), price(price_), quantity(qty_), side(side_), timestamp(ts_) {}
};

// Price level for aggregated view
struct PriceLevel {
    Price price;
    Quantity quantity;
};

}  // namespace orderbook
