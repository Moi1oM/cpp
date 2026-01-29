#pragma once
#include <thread>
#include <atomic>
#include "SPSCQueue.hpp"
#include "../v1_baseline/OrderBook.hpp"
#include "../network/MulticastReceiver.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"

namespace orderbook::v2 {

// Event structure for the lock-free queue
// Contains all information needed to process a market data update
struct OrderEvent {
    MessageType type;
    OrderId orderId;
    Price price;
    Quantity quantity;
    Side side;
    Timestamp receiveTimestamp;  // For latency measurement
};

// Lock-Free handler using SPSC queue
//
// Architecture:
// [Network Thread] --push--> [SPSC Queue] --pop--> [Processing Thread]
//
// Why this is faster:
// 1. No mutex = no kernel syscalls
// 2. No context switches = no cache pollution
// 3. Atomic operations ~20ns vs mutex ~2000ns+
// 4. Predictable latency (no lock contention spikes)
class LockFreeHandler {
public:
    LockFreeHandler(LatencyRecorder& latencyRecorder)
        : queue_(DEFAULT_QUEUE_SIZE)
        , latency_(latencyRecorder)
        , running_(false)
        , producerDone_(false)
        , messageCount_(0) {}

    ~LockFreeHandler() {
        stop();
    }

    // Start processing thread (must be called before feeding messages)
    void startConsumer() {
        running_.store(true, std::memory_order_release);
        producerDone_.store(false, std::memory_order_release);

        consumerThread_ = std::thread([this] { consumerLoop(); });
    }

    // Stop processing thread
    void stop() {
        running_.store(false, std::memory_order_release);
        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    // Signal that producer is done (for graceful shutdown)
    void signalProducerDone() {
        producerDone_.store(true, std::memory_order_release);
    }

    // Wait for consumer to finish processing all queued events
    void waitForDrain() {
        signalProducerDone();
        if (consumerThread_.joinable()) {
            consumerThread_.join();
        }
    }

    // Producer side: called by network thread or directly for benchmark
    void onPacket(const char* data, size_t len, Timestamp recvTs) {
        if (len < sizeof(MarketDataMessage)) return;

        const auto* msg = reinterpret_cast<const MarketDataMessage*>(data);

        OrderEvent event{
            .type = msg->type,
            .orderId = msg->orderId,
            .price = msg->price,
            .quantity = msg->quantity,
            .side = msg->side,
            .receiveTimestamp = recvTs
        };

        // Push to queue - no lock!
        // If queue is full, spin until space is available
        while (!queue_.push(event)) {
            // Back pressure - producer is faster than consumer
            std::this_thread::yield();
        }
    }

    // Direct push for benchmark (without parsing)
    void pushEvent(const MarketDataMessage& msg, Timestamp recvTs) {
        OrderEvent event{
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

    // Start with network receiver
    void start(MulticastReceiver& receiver) {
        startConsumer();
        receiver.run([this](const char* data, size_t len, Timestamp ts) {
            this->onPacket(data, len, ts);
        });
    }

    size_t getMessageCount() const {
        return messageCount_.load(std::memory_order_relaxed);
    }

    const v1::OrderBook& getBook() const { return book_; }

private:
    // Consumer loop - runs in dedicated thread
    void consumerLoop() {
        OrderEvent event;

        while (running_.load(std::memory_order_acquire)) {
            if (queue_.pop(event)) {
                // Record processing start time (after dequeue)
                Timestamp startProcess = getCurrentNanos();
                processEvent(event);
                // Record processing time only (excludes queue wait)
                Timestamp endProcess = getCurrentNanos();
                latency_.record(endProcess - startProcess);
                messageCount_.fetch_add(1, std::memory_order_relaxed);
            } else {
                // Queue empty
                if (producerDone_.load(std::memory_order_acquire)) {
                    // Producer is done and queue is empty - exit
                    break;
                }
                // Spin-wait for new data
                // Alternative: std::this_thread::yield() for power efficiency
            }
        }

        // Drain remaining events before exit
        while (queue_.pop(event)) {
            Timestamp startProcess = getCurrentNanos();
            processEvent(event);
            Timestamp endProcess = getCurrentNanos();
            latency_.record(endProcess - startProcess);
            messageCount_.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void processEvent(const OrderEvent& event) {
        switch (event.type) {
            case MessageType::Add: {
                Order order{
                    event.orderId,
                    event.price,
                    event.quantity,
                    event.side,
                    event.receiveTimestamp
                };
                book_.addOrder(order);
                break;
            }
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

    SPSCQueue<OrderEvent> queue_;
    v1::OrderBook book_;  // Reuse v1 OrderBook (same data structure)
    LatencyRecorder& latency_;
    std::atomic<bool> running_;
    std::atomic<bool> producerDone_;
    std::atomic<size_t> messageCount_;
    std::thread consumerThread_;
};

}  // namespace orderbook::v2
