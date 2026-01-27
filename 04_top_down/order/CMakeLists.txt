cmake_minimum_required(VERSION 3.20)
project(OrderBook CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimization flags for Release build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# V1 Baseline benchmark
add_executable(benchmark_v1
    benchmark/benchmark_v1.cpp
)
target_link_libraries(benchmark_v1 pthread)

# V2 Lock-free benchmark
add_executable(benchmark_v2
    benchmark/benchmark_v2.cpp
)
target_link_libraries(benchmark_v2 pthread)

# V3 Memory pool benchmark
add_executable(benchmark_v3
    benchmark/benchmark_v3.cpp
)
target_link_libraries(benchmark_v3 pthread)

# Compare all versions
add_executable(compare_all
    benchmark/compare_all.cpp
)
target_link_libraries(compare_all pthread)

# Main application
add_executable(orderbook_main
    main.cpp
)
target_link_libraries(orderbook_main pthread)

# Test sender (for simulation)
add_executable(test_sender
    benchmark/test_sender.cpp
)
target_link_libraries(test_sender pthread)
