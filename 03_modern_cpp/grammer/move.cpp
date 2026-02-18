#include <iostream>
#include <vector>
#include <utility> // std::move를 사용하기 위함

class MyBuffer {
private:
    int* data;
    size_t size;

public:
    // 생성자
    MyBuffer(size_t s) : size(s) {
        data = new int[size];
        std::cout << "생성자 호출 (크기: " << size << ")\n";
    }

    // 소멸자
    ~MyBuffer() {
        delete[] data;
        std::cout << "소멸자 호출 (메모리 해제)\n";
    }

    // 1. 복사 생성자 (Deep Copy)
    MyBuffer(const MyBuffer& other) : size(other.size) {
        data = new int[size];
        for (size_t i = 0; i < size; ++i) data[i] = other.data[i];
        std::cout << "복사 생성자 호출 (비싼 작업!)\n";
    }

    // 2. 이동 생성자 (Move Semantics)
    // &&는 rvalue 참조를 의미합니다.
    MyBuffer(MyBuffer&& other) noexcept : data(nullptr), size(0) {
        // 상대방의 자원을 뺏어옵니다.
        data = other.data;
        size = other.size;

        // 상대방을 빈털터리로 만듭니다 (중요!)
        other.data = nullptr;
        other.size = 0;
        std::cout << "이동 생성자 호출 (빠름!)\n";
    }
};

int main() {
    std::cout << "--- 객체 생성 ---\n";
    MyBuffer buf1(100);

    std::cout << "\n--- 복사 수행 ---\n";
    MyBuffer buf2 = buf1; // 복사 생성자 호출

    std::cout << "\n--- 이동 수행 ---\n";
    // std::move는 buf1을 rvalue로 캐스팅하여 이동 생성자가 호출되게 유도합니다.
    MyBuffer buf3 = std::move(buf1); 

    std::cout << "\n--- 프로그램 종료 ---\n";
    return 0;
}