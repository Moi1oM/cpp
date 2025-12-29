#include <thread>
#include <iostream>

void functionC() {
    std::cout << "hello from threadC - " << std::this_thread::get_id() << "\n";
}

void functionA() {
    std::cout << "hello from threadA - " << std::this_thread::get_id() << "\n";

    std::thread threadC(functionC);
    threadC.join();
}

void functionB() {
    std::cout << "hello from threadB - " << std::this_thread::get_id() << "\n";
}

// int main() {
//     std::thread threadA(functionA);
//     std::thread threadB(functionB);
//
//     threadA.join();
//     threadB.join();
//
//     std::cout << "hello from main - " << std::this_thread::get_id() << "\n";
//     return 0;
// }