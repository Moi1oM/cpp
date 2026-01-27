#pragma once
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <atomic>
#include <cmath>
#include <iostream>
#include <iomanip>
#include "Constants.hpp"

namespace orderbook {

// Get current time in nanoseconds
inline Timestamp getCurrentNanos() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
}

// Statistics structure for latency analysis
struct LatencyStats {
    uint64_t count = 0;
    uint64_t min = 0;
    uint64_t max = 0;
    double mean = 0;
    double stddev = 0;
    uint64_t p50 = 0;    // median
    uint64_t p90 = 0;
    uint64_t p99 = 0;
    uint64_t p999 = 0;   // 99.9th percentile
};

// Thread-safe latency recorder with histogram
class LatencyRecorder {
public:
    explicit LatencyRecorder(size_t capacity = BENCHMARK_MESSAGES)
        : samples_(capacity), count_(0) {}

    // Record a latency sample (nanoseconds)
    void record(uint64_t latencyNanos) {
        size_t idx = count_.fetch_add(1, std::memory_order_relaxed);
        if (idx < samples_.size()) {
            samples_[idx] = latencyNanos;
        }
    }

    // Compute statistics from recorded samples
    LatencyStats compute() const {
        size_t n = std::min(count_.load(std::memory_order_relaxed), samples_.size());
        if (n == 0) return {};

        std::vector<uint64_t> sorted(samples_.begin(), samples_.begin() + n);
        std::sort(sorted.begin(), sorted.end());

        // Calculate sum and mean
        uint64_t sum = std::accumulate(sorted.begin(), sorted.end(), 0ULL);
        double mean = static_cast<double>(sum) / n;

        // Calculate variance and stddev
        double variance = 0;
        for (size_t i = 0; i < n; ++i) {
            double diff = static_cast<double>(sorted[i]) - mean;
            variance += diff * diff;
        }
        variance /= n;

        return LatencyStats{
            .count = n,
            .min = sorted[0],
            .max = sorted[n - 1],
            .mean = mean,
            .stddev = std::sqrt(variance),
            .p50 = sorted[n * 50 / 100],
            .p90 = sorted[n * 90 / 100],
            .p99 = sorted[n * 99 / 100],
            .p999 = sorted[n * 999 / 1000]
        };
    }

    // Print formatted statistics
    void printStats(const char* label) const {
        auto stats = compute();
        std::cout << "\n=== " << label << " ===" << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Samples: " << stats.count << std::endl;
        std::cout << "Min:     " << stats.min << " ns ("
                  << stats.min / 1000.0 << " us)" << std::endl;
        std::cout << "Max:     " << stats.max << " ns ("
                  << stats.max / 1000.0 << " us)" << std::endl;
        std::cout << "Mean:    " << stats.mean << " ns ("
                  << stats.mean / 1000.0 << " us)" << std::endl;
        std::cout << "StdDev:  " << stats.stddev << " ns" << std::endl;
        std::cout << "P50:     " << stats.p50 << " ns ("
                  << stats.p50 / 1000.0 << " us)" << std::endl;
        std::cout << "P90:     " << stats.p90 << " ns ("
                  << stats.p90 / 1000.0 << " us)" << std::endl;
        std::cout << "P99:     " << stats.p99 << " ns ("
                  << stats.p99 / 1000.0 << " us)" << std::endl;
        std::cout << "P99.9:   " << stats.p999 << " ns ("
                  << stats.p999 / 1000.0 << " us)" << std::endl;
    }

    // Reset recorder for new measurements
    void reset() {
        count_.store(0, std::memory_order_relaxed);
    }

    size_t size() const {
        return count_.load(std::memory_order_relaxed);
    }

private:
    std::vector<uint64_t> samples_;
    std::atomic<size_t> count_;
};

// RAII timer for measuring scope duration
class ScopedTimer {
public:
    explicit ScopedTimer(LatencyRecorder& recorder)
        : recorder_(recorder)
        , start_(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
            end - start_).count();
        recorder_.record(duration);
    }

    // Non-copyable
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    LatencyRecorder& recorder_;
    std::chrono::high_resolution_clock::time_point start_;
};

}  // namespace orderbook
