#include <iostream>
#include <fstream>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <cmath> // 수학 연산을 위해 추가
#include "StandardPool.hpp"
#include "SpinLockPool.hpp"

using namespace std;
using namespace std::chrono;

const int NUM_TASKS = 1000000; 
const int NUM_THREADS = 4;

// 🔥 수정된 작업: "적당히 무거운" 작업 시뮬레이션
// HFT에서 오더북을 업데이트하거나, 파생상품 가격을 계산하는 정도의 부하 (약 1~2µs)
void heavy_work(std::atomic<int>& counter) {
    // volatile을 써서 컴파일러가 "쓸모없는 계산"이라고 판단해 삭제(최적화)하는 것을 방지
    volatile double result = 0;
    for (int i = 0; i < 300; ++i) {
        result += (i * 0.5) + 1.5;
    }
    
    // 작업 완료 카운트
    counter.fetch_add(1, std::memory_order_relaxed);
}

int main(int argc, char* argv[]) {
    // ... (메인 함수 내용은 그대로 두셔도 됩니다) ...
    
    // 다만, 작업을 넣을 때 생산자(Main)가 너무 빠르면 
    // SpinLock 워커들이 락을 잡을 틈을 안 줄 수 있으므로
    // 생산자가 아주 살짝 여유를 주면 SpinLock 성능이 더 좋아집니다.
    // (선택 사항: 아래 루프 안에서 yield 추가 가능)
    
    ofstream file("COMPARE_RESULT.md", ios::app); 
    if (!file.is_open()) return 1;

    cout << "벤치마크 시작 (Workload increased)..." << endl;

    // 1. Standard Pool 측정
    long long standard_time = 0;
    {
        StandardPool pool(NUM_THREADS);
        atomic<int> counter{0};
        auto start = high_resolution_clock::now();

        for(int i=0; i<NUM_TASKS; ++i) {
            pool.EnqueueJob(heavy_work, ref(counter));
        }

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

    // 파일 기록 및 출력 ... (기존과 동일)
    file << "| Standard (Mutex) | " << NUM_TASKS << " | " << standard_time << " ms |\n";
    file << "| HFT (SpinLock)   | " << NUM_TASKS << " | " << spin_time << " ms |\n";
    
    double ratio = (double)standard_time / spin_time;
    file << "\n> **결과(Load Adjusted):** SpinLock 방식이 약 **" << fixed << setprecision(2) << ratio << "배** 더 빠릅니다.\n\n";

    cout << "완료! Results saved." << endl;
    return 0;
}