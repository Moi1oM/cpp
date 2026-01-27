#pragma once
#include <mutex>
#include <thread>
#include <atomic>
#include "OrderBook.hpp"
#include "../network/MulticastReceiver.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"

namespace orderbook::v1 {

// Mutex-based handler for baseline performance measurement
//
// Why this is slow:
// 1. std::mutex::lock() can cause kernel syscall (futex) - 2000+ ns
// 2. Context switch when waiting for lock
// 3. Cache line bouncing between cores for mutex state
//
// This handler processes messages synchronously with locking
class OrderBookHandler {
public:
    OrderBookHandler(LatencyRecorder& latencyRecorder)
        : latency_(latencyRecorder)
        , running_(false)
        , messageCount_(0) {}

    // Process a single packet (called from network thread)
    void onPacket(const char* data, size_t len, Timestamp recvTs) {
        if (len < sizeof(MarketDataMessage)) return;

        const auto* msg = reinterpret_cast<const MarketDataMessage*>(data);

        // CRITICAL SECTION - This is where latency is added!
        // Every message must wait for the lock
        {
            std::lock_guard<std::mutex> lock(mutex_);
            processMessage(*msg);
        }

        // Record latency: time from receive to processing complete
        Timestamp processedTs = getCurrentNanos();
        latency_.record(processedTs - recvTs);
        messageCount_.fetch_add(1, std::memory_order_relaxed);
    }

    // Start with network receiver
    void start(MulticastReceiver& receiver) {
        running_.store(true, std::memory_order_release);
        receiver.run([this](const char* data, size_t len, Timestamp ts) {
            this->onPacket(data, len, ts);
        });
    }

    void stop() {
        running_.store(false, std::memory_order_release);
    }

    // Direct message processing (for benchmarking without network)
    void processDirectly(const MarketDataMessage& msg, Timestamp recvTs) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            processMessage(msg);
        }

        Timestamp processedTs = getCurrentNanos();
        latency_.record(processedTs - recvTs);
        messageCount_.fetch_add(1, std::memory_order_relaxed);
    }

    // Access order book (with lock for thread safety)
    template<typename Func>
    auto withOrderBook(Func&& func) {
        std::lock_guard<std::mutex> lock(mutex_);
        return func(book_);
    }

    size_t getMessageCount() const {
        return messageCount_.load(std::memory_order_relaxed);
    }

    const OrderBook& getBook() const { return book_; }

private:
    void processMessage(const MarketDataMessage& msg) {
        switch (msg.type) {
            case MessageType::Add: {
                Order order{
                    msg.orderId,
                    msg.price,
                    msg.quantity,
                    msg.side,
                    msg.exchangeTimestamp
                };
                book_.addOrder(order);
                break;
            }
            case MessageType::Modify:
                book_.modifyOrder(msg.orderId, msg.quantity);
                break;
            case MessageType::Delete:
                book_.deleteOrder(msg.orderId);
                break;
            default:
                break;
        }
    }

    OrderBook book_;
    std::mutex mutex_;  // THE BOTTLENECK - we will eliminate this in v2
    LatencyRecorder& latency_;
    std::atomic<bool> running_;
    std::atomic<size_t> messageCount_;
};

}  // namespace orderbook::v1
