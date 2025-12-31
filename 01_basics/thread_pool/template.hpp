#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

class ThreadPoolV2 {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex m_job_q;
    bool stop_all = false;
    std::condition_variable cv_job_q;

public:
    ThreadPoolV2(size_t num_threads) {
        for (size_t i = 0; i < num_threads; i++)
        {
            workers.emplace_back([this]() {this->WorkerThread();});
        }
    }
    ~ThreadPoolV2() {
        stop_all = true;
        for (auto& t : workers) t.join();
    }
    template <class F, class... Args>
    auto EnqueJob(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;

        auto job = std::make_shared<std::package_task<return_type()>>(
            [func = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable {
                return func(args...);
            }
        );
        std::future<return_type> res = job->get_future();
        {
            std::lock_guard<std::mutex> lock(m_job_q);
            jobs.push([job]() {(*job)();});
        }
        cv_job_q.notify_one();
        return res;
    }
private:
    void WorkerThread() {
        while (true)
        {
            std::unique_lock<std::mutex> lock(m_job_q);
            
            cv_job_q.wait(lock, [this]() {
                return !this->jobs.empty() || stop_all;
            });

            if (stop_all && jobs.empty()) return;

            auto job = std::move(jobs.front());
            jobs.pop();
            lock.unlock();
            job();
        }  
        
    }
};