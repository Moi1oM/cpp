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

class StandardPool {
public:
    explicit StandardPool(size_t num_threads) {
        worker_threads_.reserve(num_threads);
        for (size_t i = 0; i < num_threads; i++)
        {
            worker_threads_.emplace_back([this]() {this->WorkerThread();});
        }    
    }
    ~StandardPool() {
        {
            std::lock_guard<std::mutex> lock(m_job_q);
            stop_all_ = true;
        }
        cv_job_q_.notify_all();
    }
    template <class F, class... Args>
    auto EnqueueJob(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;

        auto job = std::make_shared<std::packaged_task<return_type()>>(
            [func = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
                return std::invoke(func, args...);
            }
        );
        std::future<return_type> res = job->get_future();

        {
            std::lock_guard<std::mutex> lock(m_job_q);
            if (stop_all_)
            {
                throw std::runtime_error("StandardPool is stopped");
            }
            jobs_.push([job]() {(*job)();});
        }
        cv_job_q_.notify_one();
        return res;
    }
private:
    void WorkerThread() {
        while (true)
        {
            std::unique_lock<std::mutex> lock(m_job_q);
            cv_job_q_.wait(lock, [this](){return !this->jobs_.empty() || stop_all_;});

            if (stop_all_ && this->jobs_.empty()) return;

            std::function<void()> job = std::move(jobs_.front());
            jobs_.pop();
            lock.unlock();

            job();
        }
    }
    // [순서 변경 중요]
    // 리소스(큐, 뮤텍스, CV)를 먼저 선언해야 나중에 파괴됩니다.
    std::queue<std::function<void()>> jobs_;
    std::condition_variable cv_job_q_;
    std::mutex m_job_q;
    bool stop_all_ = false;

    // 스레드 벡터를 가장 마지막에 선언합니다.
    // 이렇게 하면 소멸 시 가장 먼저 파괴(join)되므로, 
    // 스레드가 살아있는 동안 위쪽의 뮤텍스는 안전하게 살아있습니다.
    std::vector<std::jthread> worker_threads_;
};

int work(int t, int id) {
    std::this_thread::sleep_for(std::chrono::seconds(t));
    std::cout<< "Thread [" << id << "] finished working for " << t << "s\n";
    return t+id;
}
