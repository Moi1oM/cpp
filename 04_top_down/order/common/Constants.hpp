#pragma once
#include <cstddef>
#include <cstdint>

namespace orderbook {

// Apple M4 Pro has 128-byte cache lines
constexpr size_t CACHE_LINE_SIZE = 128;

// Queue sizes (must be power of 2 for efficient masking)
constexpr size_t DEFAULT_QUEUE_SIZE = 65536;  // 2^16

// Network constants
constexpr size_t MAX_PACKET_SIZE = 1500;      // MTU
constexpr int MULTICAST_PORT = 4950;
constexpr const char* MULTICAST_GROUP = "239.255.0.1";

// Object pool sizes
constexpr size_t ORDER_POOL_SIZE = 1 << 20;   // 1M orders pre-allocated

// Benchmark constants
constexpr size_t WARMUP_MESSAGES = 10000;
constexpr size_t BENCHMARK_MESSAGES = 1000000;

}  // namespace orderbook
