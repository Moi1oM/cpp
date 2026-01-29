#pragma once
#include <atomic>
#include <vector>
#include <cstddef>
#include <new>
#include "../common/Constants.hpp"

namespace orderbook::v3 {

// Lock-Free Object Pool with Pre-allocated Memory
//
// Why new/delete is slow:
// 1. malloc/free → brk/mmap syscalls (~500ns)
// 2. Allocator lock contention in multi-threaded code
// 3. Memory fragmentation
// 4. Unpredictable latency spikes during GC/compaction
//
// Why ObjectPool is fast:
// 1. Pre-allocate all memory upfront (one-time cost)
// 2. Lock-free allocate/deallocate using atomic CAS (~20ns)
// 3. No syscalls during operation
// 4. Predictable, constant-time allocation
//
// Implementation: Lock-free freelist using index chaining
template<typename T, size_t PoolSize = ORDER_POOL_SIZE>
class ObjectPool {
public:
    ObjectPool() {
        // Pre-allocate all memory upfront
        storage_.resize(PoolSize);

        // Build freelist: each node points to the next free index
        for (size_t i = 0; i < PoolSize - 1; ++i) {
            storage_[i].next = i + 1;
        }
        storage_[PoolSize - 1].next = INVALID_INDEX;

        freeHead_.store(0, std::memory_order_relaxed);
        allocCount_.store(0, std::memory_order_relaxed);
    }

    // Allocate an object from the pool
    // Lock-free using CAS on the freelist head
    T* allocate() {
        size_t head = freeHead_.load(std::memory_order_acquire);

        while (head != INVALID_INDEX) {
            size_t next = storage_[head].next;

            // CAS: try to move head to next
            if (freeHead_.compare_exchange_weak(
                    head, next,
                    std::memory_order_release,
                    std::memory_order_relaxed)) {
                allocCount_.fetch_add(1, std::memory_order_relaxed);
                return reinterpret_cast<T*>(&storage_[head].data);
            }
            // CAS failed, head is updated to current value, retry
        }

        return nullptr;  // Pool exhausted
    }

    // Deallocate an object back to the pool
    // Lock-free using CAS to prepend to freelist
    void deallocate(T* ptr) {
        if (!ptr) return;

        // Calculate index from pointer
        auto* node = reinterpret_cast<Node*>(
            reinterpret_cast<char*>(ptr) - offsetof(Node, data));
        size_t index = node - storage_.data();

        // Push onto freelist
        size_t head = freeHead_.load(std::memory_order_relaxed);
        do {
            storage_[index].next = head;
        } while (!freeHead_.compare_exchange_weak(
                    head, index,
                    std::memory_order_release,
                    std::memory_order_relaxed));

        allocCount_.fetch_sub(1, std::memory_order_relaxed);
    }

    // Allocate and construct in-place
    template<typename... Args>
    T* create(Args&&... args) {
        T* ptr = allocate();
        if (ptr) {
            new (ptr) T(std::forward<Args>(args)...);
        }
        return ptr;
    }

    // Destroy and deallocate
    void destroy(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr);
        }
    }

    size_t allocated() const {
        return allocCount_.load(std::memory_order_relaxed);
    }

    size_t available() const {
        return PoolSize - allocated();
    }

    static constexpr size_t capacity() { return PoolSize; }

private:
    static constexpr size_t INVALID_INDEX = ~size_t(0);

    // Node structure: either holds data or next-free index
    struct Node {
        union {
            alignas(T) char data[sizeof(T)];
            size_t next;  // Index of next free node in freelist
        };
    };

    std::vector<Node> storage_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> freeHead_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> allocCount_;
};

// RAII wrapper for pool-allocated objects
template<typename T, typename Pool>
class PoolPtr {
public:
    explicit PoolPtr(Pool& pool) : pool_(pool), ptr_(pool.allocate()) {}

    template<typename... Args>
    PoolPtr(Pool& pool, Args&&... args)
        : pool_(pool), ptr_(pool.create(std::forward<Args>(args)...)) {}

    ~PoolPtr() {
        if (ptr_) pool_.destroy(ptr_);
    }

    // Move semantics
    PoolPtr(PoolPtr&& other) noexcept
        : pool_(other.pool_), ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    PoolPtr& operator=(PoolPtr&& other) noexcept {
        if (this != &other) {
            if (ptr_) pool_.destroy(ptr_);
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // Non-copyable
    PoolPtr(const PoolPtr&) = delete;
    PoolPtr& operator=(const PoolPtr&) = delete;

    T* get() { return ptr_; }
    const T* get() const { return ptr_; }
    T* operator->() { return ptr_; }
    const T* operator->() const { return ptr_; }
    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

    T* release() {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

private:
    Pool& pool_;
    T* ptr_;
};

}  // namespace orderbook::v3
