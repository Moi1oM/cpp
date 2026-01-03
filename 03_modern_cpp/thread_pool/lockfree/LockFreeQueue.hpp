#pragma once
#include <atomic>
#include <vector>
#include <optional>
#include <type_traits>

constexpr size_t CACHE_LINE_SIZE = 128;

template<typename T>
class LockFreeQueue {
public:
    explicit LockFreeQueue(size_t buffer_size) 
        : buffer_(buffer_size), mask_(buffer_size - 1) {
        if ((buffer_size & (buffer_size-1))!=0)
        {
            throw std::runtime_error("Buffer size must be a power of 2");
        }
        for (size_t i = 0; i < buffer_size; i++)
        {
            buffer_[i].sequence.store(i, std::memory_order_relaxed);
        }
        enqueue_pos_.store(0, std::memory_order_relaxed);
        dequeue_pos_.store(0, std::memory_order_relaxed);
    }

    bool push(T data) {
        struct Cell* cell;
        size_t pos = enqueue_pos_.load(std::memory_order_acquire);

        while (true)
        {
            cell = &buffer_[ pos & mask_];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)pos;

            if (dif==0)
            {
                if (enqueue_pos_.compare_exchange_weak(pos, pos+1, std::memory_order_relaxed))
                {
                    cell->data = std::move(data);
                    cell->sequence.store(pos+1, std::memory_order_release);
                    return true;
                }
            } else if (dif < 0) {
                return false;
            } else {
                pos = enqueue_pos_.load(std::memory_order_relaxed);
            }
        }
    }

    bool pop(T& result) {
        struct Cell* cell;
        size_t pos = dequeue_pos_.load(std::memory_order_relaxed);

        while (true) {
            cell = &buffer_[pos & mask_];
            size_t seq = cell->sequence.load(std::memory_order_acquire);
            intptr_t dif = (intptr_t)seq - (intptr_t)(pos + 1);

            if (dif == 0) {
                if (dequeue_pos_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                    result = std::move(cell->data);
                    cell->sequence.store(pos + mask_ + 1, std::memory_order_release);
                    return true;
                }
            } else if (dif < 0) {
                return false; // Empty
            } else {
                pos = dequeue_pos_.load(std::memory_order_relaxed);
            }
        }
    }

private:
    struct Cell {
        std::atomic<size_t> sequence;
        T data;
    };

    alignas(CACHE_LINE_SIZE) std::vector<Cell> buffer_;
    size_t mask_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> enqueue_pos_;
    alignas(CACHE_LINE_SIZE) std::atomic<size_t> dequeue_pos_;
};