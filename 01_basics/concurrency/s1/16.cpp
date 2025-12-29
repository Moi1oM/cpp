#include <iostream>
#include <vector>
#include <numeric>  // std::accumulate
#include <thread>
#include <future>   // std::async, std::future
#include <algorithm> // std::min
#include <iterator> // std::distance, std::advance
#include <chrono>   // 성능 측정용

// [템플릿 함수] 병렬 Accumulate
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    // 1. 데이터가 너무 적으면 그냥 일반 accumulate 실행 (오버헤드 방지)
    if (!length) return init;

    unsigned long const min_per_thread = 25; // 스레드당 최소 처리량
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    // 0이 리턴될 경우를 대비해 최소 2개로 설정
    unsigned long const num_threads =
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    // 2. 각 스레드가 처리할 블록 크기 계산
    unsigned long const block_size = length / num_threads;

    // 3. 결과를 받아올 Future 벡터 생성
    // (num_threads - 1)개인 이유: 마지막 블록은 메인 스레드가 처리할 거라서
    std::vector<std::future<T>> futures(num_threads - 1);

    Iterator block_start = first;

    for(unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size); // 반복자 이동

        // 4. 비동기 작업 시작 (std::async 사용)
        // std::launch::async: 강제로 새 스레드 생성
        futures[i] = std::async(std::launch::async,
            [block_start, block_end]() {
                return std::accumulate(block_start, block_end, T());
            });

        block_start = block_end;
    }

    // 5. [중요] 메인 스레드도 놀지 않고 마지막 남은 블록을 처리함
    T last_result = std::accumulate(block_start, last, T());

    // 6. 각 스레드의 결과 취합 (Reduce 단계)
    T result = init; // 초기값부터 시작
    for(auto& f : futures) {
        result += f.get(); // 여기서 대기(Blocking) 발생, 결과 합산
    }
    result += last_result; // 메인 스레드 결과 합산

    return result;
}

// --- 성능 테스트용 Main ---
int main() {
    // 1억 개의 데이터 생성 (약 400MB)
    const long data_size = 100'000'000;
    std::vector<int> data(data_size, 1); // 모든 값 1

    std::cout << "Data size: " << data_size << " elements\n";
    std::cout << "Threads used: " << std::thread::hardware_concurrency() << "\n\n";

    // 1. 일반 std::accumulate 측정
    auto start = std::chrono::high_resolution_clock::now();
    long long sum1 = std::accumulate(data.begin(), data.end(), 0LL);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff1 = end - start;
    std::cout << "std::accumulate (Single): " << diff1.count() << " s\n";
    std::cout << "Result: " << sum1 << "\n\n";

    // 2. parallel_accumulate 측정
    start = std::chrono::high_resolution_clock::now();
    long long sum2 = parallel_accumulate(data.begin(), data.end(), 0LL);
    end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff2 = end - start;
    std::cout << "parallel_accumulate (Multi): " << diff2.count() << " s\n";
    std::cout << "Result: " << sum2 << "\n";

    std::cout << "Speedup: " << diff1.count() / diff2.count() << "x faster\n";

    return 0;
}