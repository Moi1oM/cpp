#include <iostream>
#include <chrono>
#include <random>
#include <thread>
#include <iomanip>
#include "../common/Constants.hpp"
#include "../common/Types.hpp"
#include "../common/Protocol.hpp"
#include "../common/LatencyMeasure.hpp"
#include "../v1_baseline/OrderBook.hpp"
#include "../v1_baseline/OrderBookHandler.hpp"
#include "../v2_lockfree/SPSCQueue.hpp"
#include "../v2_lockfree/LockFreeHandler.hpp"
#include "../v3_mempool/ObjectPool.hpp"
#include "../v3_mempool/OptimizedHandler.hpp"

using namespace orderbook;

class MessageGenerator {
public:
    MessageGenerator(unsigned seed = 42) : rng_(seed), nextOrderId_(1), sequence_(0) {}

    void reset() {
        rng_.seed(42);
        nextOrderId_ = 1;
        sequence_ = 0;
        activeOrders_.clear();
    }

    MarketDataMessage generateRandom() {
        std::uniform_int_distribution<int> typeDist(0, 99);
        int r = typeDist(rng_);

        if (r < 70) return generateAdd();
        if (r < 90) return generateModify();
        return generateDelete();
    }

private:
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
        return makeModifyMessage(sequence_++, activeOrders_[idxDist(rng_)], qtyDist(rng_), 0);
    }

    MarketDataMessage generateDelete() {
        if (activeOrders_.empty()) return generateAdd();
        std::uniform_int_distribution<size_t> idxDist(0, activeOrders_.size() - 1);
        size_t idx = idxDist(rng_);
        OrderId id = activeOrders_[idx];
        activeOrders_.erase(activeOrders_.begin() + idx);
        return makeDeleteMessage(sequence_++, id, 0);
    }

    std::mt19937 rng_;
    OrderId nextOrderId_;
    SequenceNumber sequence_;
    std::vector<OrderId> activeOrders_;
};

struct BenchmarkResult {
    std::string name;
    LatencyStats stats;
    double throughput;  // msg/sec
};

void printComparison(const std::vector<BenchmarkResult>& results) {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                         LATENCY COMPARISON (nanoseconds)                     ║" << std::endl;
    std::cout << "╠═══════════════════════╦═══════════╦═══════════╦═══════════╦═════════════════╣" << std::endl;
    std::cout << "║ Version               ║    P50    ║    P99    ║   P99.9   ║   Throughput    ║" << std::endl;
    std::cout << "╠═══════════════════════╬═══════════╬═══════════╬═══════════╬═════════════════╣" << std::endl;

    for (const auto& r : results) {
        std::cout << "║ " << std::left << std::setw(21) << r.name << " ║";
        std::cout << std::right << std::setw(10) << r.stats.p50 << " ║";
        std::cout << std::setw(10) << r.stats.p99 << " ║";
        std::cout << std::setw(10) << r.stats.p999 << " ║";
        std::cout << std::setw(12) << static_cast<uint64_t>(r.throughput) << " /s ║" << std::endl;
    }

    std::cout << "╚═══════════════════════╩═══════════╩═══════════╩═══════════╩═════════════════╝" << std::endl;

    // Calculate speedup
    if (results.size() >= 2) {
        std::cout << "\n=== Speedup vs V1 Baseline ===" << std::endl;
        double baselineP99 = results[0].stats.p99;
        for (size_t i = 1; i < results.size(); ++i) {
            double speedup = baselineP99 / results[i].stats.p99;
            std::cout << results[i].name << ": " << std::fixed << std::setprecision(2)
                      << speedup << "x faster (P99)" << std::endl;
        }
    }
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              ORDER BOOK LATENCY BENCHMARK - ALL VERSIONS                     ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Platform: Apple M4 Pro                                                       ║" << std::endl;
    std::cout << "║ Cache Line: " << std::setw(3) << CACHE_LINE_SIZE << " bytes                                                        ║" << std::endl;
    std::cout << "║ Messages: " << std::setw(10) << BENCHMARK_MESSAGES << "                                                    ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;

    std::vector<BenchmarkResult> results;

    // Pre-generate messages (same for all benchmarks)
    std::cout << "\nGenerating messages..." << std::endl;
    MessageGenerator generator;
    std::vector<MarketDataMessage> messages;
    messages.reserve(WARMUP_MESSAGES + BENCHMARK_MESSAGES);
    for (size_t i = 0; i < WARMUP_MESSAGES + BENCHMARK_MESSAGES; ++i) {
        messages.push_back(generator.generateRandom());
    }

    // ============ V1: Baseline (mutex) ============
    std::cout << "\n[V1] Running baseline benchmark (std::mutex)..." << std::endl;
    {
        LatencyRecorder latency(BENCHMARK_MESSAGES);
        v1::OrderBookHandler handler(latency);

        // Warmup
        for (size_t i = 0; i < WARMUP_MESSAGES; ++i) {
            Timestamp ts = getCurrentNanos();
            handler.processDirectly(messages[i], ts);
        }
        latency.reset();

        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < BENCHMARK_MESSAGES; ++i) {
            Timestamp ts = getCurrentNanos();
            handler.processDirectly(messages[WARMUP_MESSAGES + i], ts);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        results.push_back({
            "V1 (mutex)",
            latency.compute(),
            static_cast<double>(BENCHMARK_MESSAGES) * 1000 / duration
        });
        std::cout << "  Done. " << duration << " ms" << std::endl;
    }

    // ============ V2: Lock-free SPSC ============
    std::cout << "\n[V2] Running lock-free benchmark (SPSC Queue)..." << std::endl;
    {
        LatencyRecorder latency(BENCHMARK_MESSAGES);
        v2::LockFreeHandler handler(latency);
        handler.startConsumer();

        // Warmup
        for (size_t i = 0; i < WARMUP_MESSAGES; ++i) {
            Timestamp ts = getCurrentNanos();
            handler.pushEvent(messages[i], ts);
        }
        while (handler.getMessageCount() < WARMUP_MESSAGES) {
            std::this_thread::yield();
        }
        latency.reset();

        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < BENCHMARK_MESSAGES; ++i) {
            Timestamp ts = getCurrentNanos();
            handler.pushEvent(messages[WARMUP_MESSAGES + i], ts);
        }
        handler.waitForDrain();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        results.push_back({
            "V2 (lock-free)",
            latency.compute(),
            static_cast<double>(BENCHMARK_MESSAGES) * 1000 / duration
        });
        std::cout << "  Done. " << duration << " ms" << std::endl;
    }

    // ============ V3: Lock-free + Memory Pool ============
    std::cout << "\n[V3] Running optimized benchmark (Lock-free + Pool)..." << std::endl;
    {
        LatencyRecorder latency(BENCHMARK_MESSAGES);
        v3::OptimizedHandler handler(latency);
        handler.startConsumer();

        // Warmup
        for (size_t i = 0; i < WARMUP_MESSAGES; ++i) {
            Timestamp ts = getCurrentNanos();
            handler.pushEvent(messages[i], ts);
        }
        while (handler.getMessageCount() < WARMUP_MESSAGES) {
            std::this_thread::yield();
        }
        latency.reset();

        // Benchmark
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < BENCHMARK_MESSAGES; ++i) {
            Timestamp ts = getCurrentNanos();
            handler.pushEvent(messages[WARMUP_MESSAGES + i], ts);
        }
        handler.waitForDrain();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        results.push_back({
            "V3 (lock-free+pool)",
            latency.compute(),
            static_cast<double>(BENCHMARK_MESSAGES) * 1000 / duration
        });
        std::cout << "  Done. " << duration << " ms" << std::endl;
    }

    // Print comparison
    printComparison(results);

    // Detailed stats for each version
    std::cout << "\n=== Detailed Statistics ===" << std::endl;
    for (const auto& r : results) {
        std::cout << "\n--- " << r.name << " ---" << std::endl;
        std::cout << "Min:    " << r.stats.min << " ns" << std::endl;
        std::cout << "Max:    " << r.stats.max << " ns" << std::endl;
        std::cout << "Mean:   " << std::fixed << std::setprecision(2) << r.stats.mean << " ns" << std::endl;
        std::cout << "StdDev: " << r.stats.stddev << " ns" << std::endl;
    }

    return 0;
}
