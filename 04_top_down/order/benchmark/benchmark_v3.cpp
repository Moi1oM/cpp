#include <iostream>
#include <chrono>
#include <random>
#include <thread>
#include "../common/Constants.hpp"
#include "../common/Types.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"
#include "../v3_mempool/ObjectPool.hpp"
#include "../v3_mempool/OptimizedHandler.hpp"

using namespace orderbook;
using namespace orderbook::v3;

class MessageGenerator {
public:
    MessageGenerator() : rng_(42), nextOrderId_(1), sequence_(0) {}

    MarketDataMessage generateAdd() {
        std::uniform_int_distribution<int> priceDist(9900, 10100);
        std::uniform_int_distribution<int> qtyDist(1, 1000);
        std::uniform_int_distribution<int> sideDist(0, 1);

        Price price = priceDist(rng_) * 100;
        Quantity qty = qtyDist(rng_);
        Side side = sideDist(rng_) == 0 ? Side::Buy : Side::Sell;

        activeOrders_.push_back(nextOrderId_);

        return makeAddMessage(sequence_++, nextOrderId_++, price, qty, side, 0);
    }

    MarketDataMessage generateModify() {
        if (activeOrders_.empty()) return generateAdd();

        std::uniform_int_distribution<size_t> idxDist(0, activeOrders_.size() - 1);
        std::uniform_int_distribution<int> qtyDist(1, 500);

        return makeModifyMessage(sequence_++, activeOrders_[idxDist(rng_)],
                                 qtyDist(rng_), 0);
    }

    MarketDataMessage generateDelete() {
        if (activeOrders_.empty()) return generateAdd();

        std::uniform_int_distribution<size_t> idxDist(0, activeOrders_.size() - 1);
        size_t idx = idxDist(rng_);
        OrderId id = activeOrders_[idx];
        activeOrders_.erase(activeOrders_.begin() + idx);

        return makeDeleteMessage(sequence_++, id, 0);
    }

    MarketDataMessage generateRandom() {
        std::uniform_int_distribution<int> typeDist(0, 99);
        int r = typeDist(rng_);

        if (r < 70) return generateAdd();
        if (r < 90) return generateModify();
        return generateDelete();
    }

private:
    std::mt19937 rng_;
    OrderId nextOrderId_;
    SequenceNumber sequence_;
    std::vector<OrderId> activeOrders_;
};

int main() {
    std::cout << "=== V3 Memory Pool Benchmark (Lock-Free + Object Pool) ===" << std::endl;
    std::cout << "Platform: Apple M4 Pro" << std::endl;
    std::cout << "Cache Line: " << CACHE_LINE_SIZE << " bytes" << std::endl;
    std::cout << "Queue Size: " << DEFAULT_QUEUE_SIZE << std::endl;
    std::cout << "Pool Size: " << ORDER_POOL_SIZE << std::endl;
    std::cout << "Messages: " << BENCHMARK_MESSAGES << std::endl;
    std::cout << "Warmup: " << WARMUP_MESSAGES << std::endl;
    std::cout << std::endl;

    LatencyRecorder latency(BENCHMARK_MESSAGES);
    OptimizedHandler handler(latency);
    MessageGenerator generator;

    // Pre-generate all messages
    std::cout << "Generating messages..." << std::endl;
    std::vector<MarketDataMessage> messages;
    messages.reserve(WARMUP_MESSAGES + BENCHMARK_MESSAGES);
    for (size_t i = 0; i < WARMUP_MESSAGES + BENCHMARK_MESSAGES; ++i) {
        messages.push_back(generator.generateRandom());
    }

    // Start consumer thread
    std::cout << "Starting consumer thread..." << std::endl;
    handler.startConsumer();

    // Warmup phase
    std::cout << "Warming up..." << std::endl;
    for (size_t i = 0; i < WARMUP_MESSAGES; ++i) {
        Timestamp ts = getCurrentNanos();
        handler.pushEvent(messages[i], ts);
    }

    // Wait for warmup to complete
    while (handler.getMessageCount() < WARMUP_MESSAGES) {
        std::this_thread::yield();
    }
    latency.reset();

    // Benchmark phase
    std::cout << "Running benchmark..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < BENCHMARK_MESSAGES; ++i) {
        Timestamp ts = getCurrentNanos();
        handler.pushEvent(messages[WARMUP_MESSAGES + i], ts);
    }

    // Wait for all messages to be processed
    handler.waitForDrain();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime).count();

    // Print results
    latency.printStats("V3 - Lock-Free + Object Pool");

    std::cout << "\n=== Throughput ===" << std::endl;
    std::cout << "Total time: " << totalDuration << " ms" << std::endl;
    std::cout << "Throughput: " << (BENCHMARK_MESSAGES * 1000 / totalDuration)
              << " msg/sec" << std::endl;

    std::cout << "\n=== Order Book State ===" << std::endl;
    std::cout << "Orders: " << handler.getBook().orderCount() << std::endl;
    std::cout << "Bid levels: " << handler.getBook().bidLevelCount() << std::endl;
    std::cout << "Ask levels: " << handler.getBook().askLevelCount() << std::endl;
    std::cout << "Pool allocated: " << handler.getBook().poolAllocated() << std::endl;
    std::cout << "Pool available: " << handler.getBook().poolAvailable() << std::endl;

    if (auto bid = handler.getBook().bestBid()) {
        std::cout << "Best bid: " << bid->price / 100.0 << " x " << bid->quantity << std::endl;
    }
    if (auto ask = handler.getBook().bestAsk()) {
        std::cout << "Best ask: " << ask->price / 100.0 << " x " << ask->quantity << std::endl;
    }

    return 0;
}
