#include <iostream>
#include <fstream>
#include <chrono>
#include <atomic>
#include <iomanip>
#include "StandardPool.hpp"
#include "SpinLockPool.hpp"

using namespace std;
using namespace std::chrono;

const int NUM_TASKS = 1000000; // 100만 건
const int NUM_THREADS = 4;     // 코어 수에 맞춰 조정

// 테스트할 작업 (아주 가벼운 연산 -> 락 오버헤드 측정용)
void heavy_work(std::atomic<int>& counter) {
    counter.fetch_add(1, std::memory_order_relaxed);
}

int main(int argc, char* argv[]) {
    // 결과 파일 열기 (append 모드)
    ofstream file("COMPARE_RESULT.md", ios::app); 
    if (!file.is_open()) {
        cerr << "파일을 열 수 없습니다." << endl;
        return 1;
    }

    cout << "벤치마크 시작..." << endl;

    // 1. Standard Pool 측정
    long long standard_time = 0;
    {
        StandardPool pool(NUM_THREADS);
        atomic<int> counter{0};
        auto start = high_resolution_clock::now();

        for(int i=0; i<NUM_TASKS; ++i) {
            pool.EnqueueJob(heavy_work, ref(counter));
        }

        // 완료 대기 (간단한 busy wait)
        while(counter.load() < NUM_TASKS) {
            this_thread::yield();
        }
        auto end = high_resolution_clock::now();
        standard_time = duration_cast<milliseconds>(end - start).count();
    }

    // 2. SpinLock Pool 측정
    long long spin_time = 0;
    {
        SpinLockPool pool(NUM_THREADS);
        atomic<int> counter{0};
        auto start = high_resolution_clock::now();

        for(int i=0; i<NUM_TASKS; ++i) {
            pool.EnqueueJob(heavy_work, ref(counter));
        }

        while(counter.load() < NUM_TASKS) {
            // SpinWait
        }
        auto end = high_resolution_clock::now();
        spin_time = duration_cast<milliseconds>(end - start).count();
    }

    // 파일에 기록
    file << "| Standard (Mutex) | " << NUM_TASKS << " | " << standard_time << " ms |\n";
    file << "| HFT (SpinLock)   | " << NUM_TASKS << " | " << spin_time << " ms |\n";
    
    // 성능 배수 계산
    double ratio = (double)standard_time / spin_time;
    file << "\n> **결과:** SpinLock 방식이 약 **" << fixed << setprecision(2) << ratio << "배** 더 빠릅니다.\n\n";

    cout << "완료! COMPARE_RESULT.md 파일을 확인하세요." << endl;
    return 0;
}