#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <type_traits>
#include <chrono>

class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        while(flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }
    void unlock(){
        flag.clear(std::memory_order_release);
    }
};

class SpinLockPool {
public:
    explicit SpinLockPool(size_t num_threads) {
        worker_threads_.reserve(num_threads);
        for (size_t i = 0; i < num_threads; i++)
        {
            worker_threads_.emplace_back([this, i]() {this->WorkerThread(i);});
        }
    }

    ~SpinLockPool() {
        stop_all_.store(true, std::memory_order_release);
    }

    template <class F, class... Args>
    auto EnqueueJob(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;

        auto job = std::make_shared<std::packaged_task<return_type()>>(
            [func = std::forward<F>(f), ...args=std::forward<Args>(args)] () mutable {
                return std::invoke(func, args...);
            }
        );

        std::future<return_type> res = job->get_future();
        {
            std::lock_guard<SpinLock> lock(spin_lock_);
            jobs_.push([job]() {(*job)();});
        }
        return res;
    }
private:
    void WorkerThread(int id) {
        while (!stop_all_.load(std::memory_order_acquire)) {
            std::function<void()> job;
            bool found_job = false;
            {
                std::lock_guard<SpinLock> lock(spin_lock_);
                if (!jobs_.empty())
                {
                    job = std::move(jobs_.front());
                    jobs_.pop();
                    found_job = true;
                }
            }
            if (found_job)
            {
                job();
            } else {
                // 일이 없으면?
                // 일반적인 ThreadPool: cv.wait()로 잠듦 (OS 스케줄러 개입)
                // HFT ThreadPool: 그냥 루프 다시 돔 (Busy Wait)
                
                // 너무 맹렬하게 돌면 메모리 버스에 부하를 줄 수 있으므로 
                // 아주 살짝 힌트를 주기도 함 (std::this_thread::yield() 등)
                // 하지만 극단적 HFT는 이조차도 안 하고 계속 큐를 쪼아댑니다.
            }
        }
    }

    std::vector<std::jthread> worker_threads_;
    std::queue<std::function<void()>> jobs_;

    SpinLock spin_lock_;
    std::atomic<bool> stop_all_{false};
};