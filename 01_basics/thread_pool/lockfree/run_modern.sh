#!/bin/bash

OUTPUT="MODERN_RESULT.md"

echo "Running C++20 Benchmark..."

# -std=c++20 필수!
g++ -std=c++20 -O3 -pthread benchmark_modern.cpp -o modern_bench

if [ $? -eq 0 ]; then
    ./modern_bench | tee $OUTPUT
else
    echo "Compilation Failed!"
fi