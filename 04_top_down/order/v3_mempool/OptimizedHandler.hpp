#pragma once
#include <thread>
#include <atomic>
#include <unordered_map>
#include <map>
#include "ObjectPool.hpp"
#include "../v2_lockfree/SPSCQueue.hpp"
#include "../common/Types.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"

namespace orderbook::v3 {

// Event structure with pool-allocated order pointer
struct PooledOrderEvent {
    MessageType type;
    OrderId orderId;
    Price price;
    Quantity quantity;
    Side side;
    Timestamp receiveTimestamp;
};

// Optimized Order Book using Object Pool
// - No new/delete during operation
// - Orders are allocated from pre-reserved pool
class OptimizedOrderBook {
public:
    OptimizedOrderBook() : orderPool_() {}

    void addOrder(OrderId id, Price price, Quantity qty, Side side, Timestamp ts) {
        // Allocate from pool instead of new
        Order* order = orderPool_.create(id, price, qty, side, ts);
        if (!order) return;  // Pool exhausted

        orders_[id] = order;

        auto& book = (side == Side::Buy) ? bids_ : asks_;
        book[price] += qty;
    }

    void modifyOrder(OrderId id, Quantity newQty) {
        auto it = orders_.find(id);
        if (it == orders_.end()) return;

        Order* order = it->second;
        auto& book = (order->side == Side::Buy) ? bids_ : asks_;

        book[order->price] -= order->quantity;
        book[order->price] += newQty;

        if (book[order->price] == 0) {
            book.erase(order->price);
        }

        order->quantity = newQty;
    }

    void deleteOrder(OrderId id) {
        auto it = orders_.find(id);
        if (it == orders_.end()) return;

        Order* order = it->second;
        auto& book = (order->side == Side::Buy) ? bids_ : asks_;

        book[order->price] -= order->quantity;
        if (book[order->price] == 0) {
            book.erase(order->price);
        }

        // Return to pool instead of delete
        orderPool_.destroy(order);
        orders_.erase(it);
    }

    std::optional<PriceLevel> bestBid() const {
        if (bids_.empty()) return std::nullopt;
        auto it = bids_.rbegin();
        return PriceLevel{it->first, it->second};
    }

    std::optional<PriceLevel> bestAsk() const {
        if (asks_.empty()) return std::nullopt;
        auto it = asks_.begin();
        return PriceLevel{it->first, it->second};
    }

    size_t orderCount() const { return orders_.size(); }
    size_t bidLevelCount() const { return bids_.size(); }
    size_t askLevelCount() const { return asks_.size(); }
    size_t poolAllocated() const { return orderPool_.allocated(); }
    size_t poolAvailable() const { return orderPool_.available(); }

private:
    ObjectPool<Order, ORDER_POOL_SIZE> orderPool_;
    std::unordered_map<OrderId, Order*> orders_;
    std::map<Price, Quantity> bids_;
    std::map<Price, Quantity> asks_;
};

// Fully optimized handler: Lock-free queue + Object Pool
class OptimizedHandler {
public:
    OptimizedHandler(LatencyRecorder& latencyRecorder)
        : queue_(DEFAULT_QUEUE_SIZE)
        , latency_(latencyRecorder)
        , running_(false)
        , producerDone_(false)
        , messageCount_(0) {}

    ~OptimizedHandler() {
        stop();
    }

    void startConsumer() {
        running_.store(true, std::memory_order_release);
        producerDone_.store(false, std::memory_order_release);
        consumerThread_ = std::thread([this] { consumerLoop(); });
    }

    void stop() {
        running_.store(false, std::memory_order_release);
        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    void signalProducerDone() {
        producerDone_.store(true, std::memory_order_release);
    }

    void waitForDrain() {
        signalProducerDone();
        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    void pushEvent(const MarketDataMessage& msg, Timestamp recvTs) {
        PooledOrderEvent event{
            .type = msg.type,
            .orderId = msg.orderId,
            .price = msg.price,
            .quantity = msg.quantity,
            .side = msg.side,
            .receiveTimestamp = recvTs
        };

        while (!queue_.push(event)) {
            std::this_thread::yield();
        }
    }

    size_t getMessageCount() const {
        return messageCount_.load(std::memory_order_relaxed);
    }

    const OptimizedOrderBook& getBook() const { return book_; }

private:
    void consumerLoop() {
        PooledOrderEvent event;

        while (running_.load(std::memory_order_acquire)) {
            if (queue_.pop(event)) {
                // Record processing time only (after dequeue)
                Timestamp startProcess = getCurrentNanos();
                processEvent(event);
                Timestamp endProcess = getCurrentNanos();
                latency_.record(endProcess - startProcess);
                messageCount_.fetch_add(1, std::memory_order_relaxed);
            } else {
                if (producerDone_.load(std::memory_order_acquire)) {
                    break;
                }
            }
        }

        // Drain remaining
        while (queue_.pop(event)) {
            Timestamp startProcess = getCurrentNanos();
            processEvent(event);
            Timestamp endProcess = getCurrentNanos();
            latency_.record(endProcess - startProcess);
            messageCount_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void processEvent(const PooledOrderEvent& event) {
        switch (event.type) {
            case MessageType::Add:
                book_.addOrder(event.orderId, event.price, event.quantity,
                              event.side, event.receiveTimestamp);
                break;
            case MessageType::Modify:
                book_.modifyOrder(event.orderId, event.quantity);
                break;
            case MessageType::Delete:
                book_.deleteOrder(event.orderId);
                break;
            default:
                break;
        }
    }

    v2::SPSCQueue<PooledOrderEvent> queue_;
    OptimizedOrderBook book_;
    LatencyRecorder& latency_;
    std::atomic<bool> running_;
    std::atomic<bool> producerDone_;
    std::atomic<size_t> messageCount_;
    std::thread consumerThread_;
};

}  // namespace orderbook::v3
