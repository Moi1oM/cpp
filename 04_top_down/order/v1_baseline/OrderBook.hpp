#pragma once
#include <map>
#include <unordered_map>
#include <optional>
#include "../common/Types.hpp"

namespace orderbook::v1 {

// Simple std::map based Order Book
// - Bids: sorted descending (highest price = best bid)
// - Asks: sorted ascending (lowest price = best ask)
//
// Why std::map?
// - Red-black tree: O(log n) insert/delete/lookup
// - Sorted keys: easy to find best bid/ask
// - Trade-off: cache-unfriendly, heap allocations
class OrderBook {
public:
    void addOrder(const Order& order) {
        orders_[order.id] = order;

        auto& book = (order.side == Side::Buy) ? bids_ : asks_;
        book[order.price] += order.quantity;
    }

    void modifyOrder(OrderId id, Quantity newQuantity) {
        auto it = orders_.find(id);
        if (it == orders_.end()) return;

        Order& order = it->second;
        auto& book = (order.side == Side::Buy) ? bids_ : asks_;

        // Update price level
        Quantity oldQty = order.quantity;
        book[order.price] -= oldQty;
        book[order.price] += newQuantity;

        // Remove price level if empty
        if (book[order.price] == 0) {
            book.erase(order.price);
        }

        order.quantity = newQuantity;
    }

    void deleteOrder(OrderId id) {
        auto it = orders_.find(id);
        if (it == orders_.end()) return;

        const Order& order = it->second;
        auto& book = (order.side == Side::Buy) ? bids_ : asks_;

        book[order.price] -= order.quantity;
        if (book[order.price] == 0) {
            book.erase(order.price);
        }

        orders_.erase(it);
    }

    // Get best bid (highest buy price)
    std::optional<PriceLevel> bestBid() const {
        if (bids_.empty()) return std::nullopt;
        auto it = bids_.rbegin();  // reverse iterator = highest price
        return PriceLevel{it->first, it->second};
    }

    // Get best ask (lowest sell price)
    std::optional<PriceLevel> bestAsk() const {
        if (asks_.empty()) return std::nullopt;
        auto it = asks_.begin();   // forward iterator = lowest price
        return PriceLevel{it->first, it->second};
    }

    // Get spread (ask - bid)
    std::optional<Price> spread() const {
        auto bid = bestBid();
        auto ask = bestAsk();
        if (!bid || !ask) return std::nullopt;
        return ask->price - bid->price;
    }

    // Get order by ID
    const Order* getOrder(OrderId id) const {
        auto it = orders_.find(id);
        return (it != orders_.end()) ? &it->second : nullptr;
    }

    // Statistics
    size_t orderCount() const { return orders_.size(); }
    size_t bidLevelCount() const { return bids_.size(); }
    size_t askLevelCount() const { return asks_.size(); }

    void clear() {
        orders_.clear();
        bids_.clear();
        asks_.clear();
    }

private:
    // Individual orders by ID
    std::unordered_map<OrderId, Order> orders_;

    // Aggregated price levels
    // Using std::map for sorted order
    std::map<Price, Quantity> bids_;  // Buy orders
    std::map<Price, Quantity> asks_;  // Sell orders
};

}  // namespace orderbook::v1
