#pragma once

#include <vector>
#include <thread>

class thread_guard {
    std::thread &t;

    public:
    explicit thread_guard(std::thread &t) : t(t) {}

    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }

    thread_guard(const thread_guard &) = delete;
    thread_guard &operator=(const thread_guard &) = delete;
};