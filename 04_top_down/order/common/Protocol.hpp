#pragma once
#include <cstdint>
#include "Types.hpp"

namespace orderbook {

// Compact wire format (no padding) for network transmission
// Using pragma pack to ensure exact byte layout
#pragma pack(push, 1)
struct MarketDataMessage {
    MessageType type;           // 1 byte
    SequenceNumber sequence;    // 8 bytes
    OrderId orderId;            // 8 bytes
    Price price;                // 8 bytes
    Quantity quantity;          // 8 bytes
    Side side;                  // 1 byte
    Timestamp exchangeTimestamp; // 8 bytes
    // Total: 42 bytes
};
#pragma pack(pop)

static_assert(sizeof(MarketDataMessage) == 42, "Wire format size check failed");

// Helper to create messages
inline MarketDataMessage makeAddMessage(
    SequenceNumber seq, OrderId id, Price price, Quantity qty, Side side, Timestamp ts) {
    return MarketDataMessage{
        .type = MessageType::Add,
        .sequence = seq,
        .orderId = id,
        .price = price,
        .quantity = qty,
        .side = side,
        .exchangeTimestamp = ts
    };
}

inline MarketDataMessage makeModifyMessage(
    SequenceNumber seq, OrderId id, Quantity newQty, Timestamp ts) {
    return MarketDataMessage{
        .type = MessageType::Modify,
        .sequence = seq,
        .orderId = id,
        .price = 0,  // Not used for modify
        .quantity = newQty,
        .side = Side::Buy,  // Not used
        .exchangeTimestamp = ts
    };
}

inline MarketDataMessage makeDeleteMessage(
    SequenceNumber seq, OrderId id, Timestamp ts) {
    return MarketDataMessage{
        .type = MessageType::Delete,
        .sequence = seq,
        .orderId = id,
        .price = 0,
        .quantity = 0,
        .side = Side::Buy,
        .exchangeTimestamp = ts
    };
}

}  // namespace orderbook
