#pragma once
#include "LockFreeQueue.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <concepts>

class ModernThreadPool {
public:
    ModernThreadPool(size_t num_threads, size_t queue_size = 65536) 
        :tasks_(queue_size) {
        workers_.reserve(num_threads);
        for (size_t i = 0; i < num_threads; i++)
        {
            workers_.emplace_back([this](std::stop_token st) {
                this->WorkerThread(st);
            });
        }
    }

    ~ModernThreadPool() = default;

    template <typename F, typename... Args>
    requires std::invocable<F, Args...>
    auto EnqueueJob(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            [func=std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
                return std::invoke(func, args...);
            }
        );

        std::future<return_type> res = task->get_future();

        while (!tasks_.push([task](){(*task)();}))
        {
            std::this_thread::yield();
        }
        return res;
    }
private:
    void WorkerThread(std::stop_token st) {
        while (!st.stop_requested())
        {
            std::function<void()> task;
            if (tasks_.pop(task))
            {
                task();
            } else {
                std::this_thread::yield();
            }
            
        }
        
    }

    LockFreeQueue<std::function<void()>> tasks_;
    std::vector<std::jthread> workers_;
};