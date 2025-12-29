#include<iostream>
#include<thread>

class callable_class {
public:
    void operator()(){
        printf("Hello from class with function call operator - %d\n", std::this_thread::get_id());
    }
};

void foo () {
    printf("foo from foo - %d\n", std::this_thread::get_id());
}

void run(){
    std::thread thread1(foo);

    callable_class obj;
    std::thread thread2(obj);

    std::thread thread3([] {
        printf("Hello from lambda - %d\n", std::this_thread::get_id());
    });

    thread1.join();
    thread2.join();
    thread3.join();

    printf("hello from run - %d\n", std::this_thread::get_id());
}