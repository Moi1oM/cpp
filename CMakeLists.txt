cmake_minimum_required(VERSION 4.0)
project(cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp
        01_basics/concurrency/s1/4.cpp
        01_basics/concurrency/s1/5.cpp
        01_basics/concurrency/s1/common.h
        01_basics/concurrency/s1/9.cpp
        01_basics/concurrency/s1/16.cpp
        01_basics/thread_pool/lockfree/ModernThreadPool.hpp)
