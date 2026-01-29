#pragma once
#include <atomic>
#include <vector>
#include <stdexcept>
#include "../common/Constants.hpp"

namespace orderbook::v2 {

// Single-Producer Single-Consumer Lock-Free Queue
//
// Why SPSC is faster than MPMC:
// - No CAS retry loops (only one writer, one reader)
// - Simpler memory ordering requirements
// - Perfect for network thread -> processing thread pipeline
//
// Memory Layout:
// - head_ and tail_ on separate cache lines to prevent false sharing
// - Producer only writes head_, reads tail_
// - Consumer only writes tail_, reads head_
template<typename T>
class SPSCQueue {
public:
    explicit SPSCQueue(size_t capacity = DEFAULT_QUEUE_SIZE)
        : buffer_(capacity)
        , capacity_(capacity)
        , mask_(capacity - 1) {
        // Capacity must be power of 2 for efficient masking
        if ((capacity & (capacity - 1)) != 0) {
            throw std::runtime_error("Capacity must be power of 2");
        }
    }

    // Producer side - called by network thread
    // Returns false if queue is full
    bool push(const T& item) {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next = (head + 1) & mask_;

        // Check if full (next position equals tail)
        // Use acquire to see consumer's latest tail update
        if (next == tail_.load(std::memory_order_acquire)) {
            return false;  // Queue full
        }

        // Write data
        buffer_[head] = item;

        // Publish: make data visible before updating head
        // Use release so consumer sees the data when it reads head
        head_.store(next, std::memory_order_release);
        return true;
    }

    // Move version for efficiency
    bool push(T&& item) {
        size_t head = head_.load(std::memory_order_relaxed);
        size_t next = (head + 1) & mask_;

        if (next == tail_.load(std::memory_order_acquire)) {
            return false;
        }

        buffer_[head] = std::move(item);
        head_.store(next, std::memory_order_release);
        return true;
    }

    // Consumer side - called by processing thread
    // Returns false if queue is empty
    bool pop(T& item) {
        size_t tail = tail_.load(std::memory_order_relaxed);

        // Check if empty (tail equals head)
        // Use acquire to see producer's latest head update and data
        if (tail == head_.load(std::memory_order_acquire)) {
            return false;  // Queue empty
        }

        // Read data
        item = std::move(buffer_[tail]);

        // Publish: update tail to free the slot for producer
        // Use release so producer sees updated tail
        tail_.store((tail + 1) & mask_, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

    size_t size() const {
        size_t head = head_.load(std::memory_order_acquire);
        size_t tail = tail_.load(std::memory_order_acquire);
        return (head - tail + capacity_) & mask_;
    }

    size_t capacity() const { return capacity_ - 1; }  // One slot reserved

private:
    std::vector<T> buffer_;
    const size_t capacity_;
    const size_t mask_;

    // Separate cache lines to prevent false sharing
    // Producer writes head_, consumer writes tail_
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> head_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> tail_{0};
};

}  // namespace orderbook::v2
