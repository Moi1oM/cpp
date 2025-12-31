#include <queue>
#include <functional>
#include <mutex>
#include <thread>

class ThreadPoolV2 {
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobs;
    std::mutex m_job_q;
    bool stop_all = false;

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
    void EnqueJob(std::function<void()> job) {
        std::lock_guard<std::mutex> lock(m_job_q);
        jobs.push(job);
    }
private:
    void WorkerThread() {
        while (!stop_all)
        {
            std::unique_lock<std::mutex> lock(m_job_q);
            if (!jobs.empty())
            {
                auto job = jobs.front();
                jobs.pop();
                lock.unlock();
                job();
            } else {
                lock.unlock();
            }
        }
        
    }
};