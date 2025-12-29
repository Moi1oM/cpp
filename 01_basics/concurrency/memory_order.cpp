#include <atomic>
#include <cstdio>
#include <thread>
#include <vector>
using std::memory_order_relaxed;

void t1(std::atomic<int>* a, std::atomic<int>* b) {
  b->store(1, memory_order_relaxed);      // b = 1 (쓰기)
  int x = a->load(memory_order_relaxed);  // x = a (읽기)

  printf("x : %d \n", x);
}

void t2(std::atomic<int>* a, std::atomic<int>* b) {
  a->store(1, memory_order_relaxed);      // a = 1 (쓰기)
  int y = b->load(memory_order_relaxed);  // y = b (읽기)

  printf("y : %d \n", y);
}

int main() {
  std::vector<std::thread> threads;

  std::atomic<int> a(0);
  std::atomic<int> b(0);

  threads.push_back(std::thread(t1, &a, &b));
  threads.push_back(std::thread(t2, &a, &b));

  for (int i = 0; i < 2; i++) {
    threads[i].join();
  }
}