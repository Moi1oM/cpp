#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <functional>
#include <future>
#include <type_traits>

// SpinLock은 그대로 둡니다 (yield 포함)
class MySpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
             std::this_thread::yield(); 
        }
    }
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

class SpinLockPool {
public:
    explicit SpinLockPool(size_t num_threads) {
        stop_all.store(false);
        for(size_t i = 0; i < num_threads; ++i)
            workers.emplace_back([this] {
                while(!stop_all.load(std::memory_order_relaxed)) {
                    // [핵심 최적화 1] 
                    // 락을 잡기 전에, 일이 있는지 먼저 "눈으로만" 확인합니다 (Load).
                    // q_size_는 atomic이라 락 없이 읽어도 안전합니다.
                    // 일이 없으면? 락 근처에도 가지 않고 쉽니다. (캐시 바운싱 방지)
                    if (q_size_.load(std::memory_order_relaxed) == 0) {
                        std::this_thread::yield();
                        continue;
                    }

                    std::function<void()> task;
                    bool has_task = false;
                    
                    // 일이 있는 것 같으니 그제서야 진입 시도
                    {
                        std::lock_guard<MySpinLock> lock(spin_lock);
                        if(!tasks.empty()) {
                            task = std::move(tasks.front());
                            tasks.pop();
                            
                            // [핵심 최적화 2] 사이즈 감소
                            q_size_.fetch_sub(1, std::memory_order_relaxed);
                            has_task = true;
                        }
                    }
                    if(has_task) task();
                }
            });
    }

    template<class F, class... Args>
    auto EnqueueJob(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
             [func = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
                return std::invoke(func, args...);
            }
        );
        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<MySpinLock> lock(spin_lock);
            tasks.emplace([task](){ (*task)(); });
            
            // [핵심 최적화 3] 사이즈 증가
            q_size_.fetch_add(1, std::memory_order_relaxed);
        }
        return res;
    }

    ~SpinLockPool() {
        stop_all.store(true, std::memory_order_release);
        for(auto& worker : workers) if(worker.joinable()) worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    MySpinLock spin_lock;
    std::atomic<bool> stop_all;
    
    // [New] 락 없이 대기하기 위한 원자적 카운터
    std::atomic<size_t> q_size_{0};
};