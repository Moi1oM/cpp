#include<iostream>
#include<vector>
#include<thread>

class ThreadPoolMVP {
    std::vector<std::thread> workers;

public:
    ThreadPoolMVP(size_t num_threads) {
        for (size_t i = 0; i < num_threads; i++)
        {
            workers.emplace_back([i]() {
                std::cout << "일꾼 " << i << " 출근 완료!\n";
            });
        }
    }

    ~ThreadPoolMVP() {
        for (auto &t : workers)
        {
            if (t.joinable())
            {
                t.join();
            }
            
        }
        
    }
};

int main(){
    ThreadPoolMVP pool(3);
    return 0;
}