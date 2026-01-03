#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <iomanip>
#include "ModernThreadPool.hpp"

using namespace std;
using namespace std::chrono;

const int NUM_TASKS = 1000000;
const int NUM_THREADS = 4;

// 부하 테스트용 작업
void work_load(std::atomic<int>& counter) {
    volatile double result = 0;
    for (int i = 0; i < 100; ++i) {
        result += (i * 0.5) + 1.5;
    }
    counter.fetch_add(1, std::memory_order_relaxed);
}

int main() {
    cout << "🚀 C++20 Modern Lock-Free Pool Benchmark 🚀" << endl;
    cout << "Running on: Apple M4 Pro (Optimized for ARM64)" << endl;
    
    // 큐 사이즈는 2의 거듭제곱으로 (예: 2^16 = 65536)
    ModernThreadPool pool(NUM_THREADS, 65536); 
    
    std::atomic<int> counter{0};
    
    auto start = high_resolution_clock::now();

    for(int i = 0; i < NUM_TASKS; ++i) {
        pool.EnqueueJob(work_load, ref(counter));
    }

    // 메인 쓰레드 대기 (Busy Wait)
    while(counter.load() < NUM_TASKS) {
        // Main 쓰레드도 멍때리지 말고 yield
        std::this_thread::yield();
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();

    cout << "------------------------------------------------" << endl;
    cout << "Tasks       : " << NUM_TASKS << endl;
    cout << "Time        : " << duration << " ms" << endl;
    cout << "Throughput  : " << (double)NUM_TASKS / duration * 1000 << " ops/sec" << endl;
    cout << "------------------------------------------------" << endl;

    return 0;
}