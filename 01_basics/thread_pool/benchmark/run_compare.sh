#!/bin/bash

OUTPUT="COMPARE_RESULT.md"

# 1. 파일 초기화 및 헤더 작성
echo "# ⚔️ ThreadPool Performance Battle" > $OUTPUT
echo "### System Spec" >> $OUTPUT
echo "- **Date**: $(date)" >> $OUTPUT

# Mac CPU 정보 가져오기
if [[ "$OSTYPE" == "darwin"* ]]; then
    CPU=$(sysctl -n machdep.cpu.brand_string)
else
    CPU=$(grep -m 1 'model name' /proc/cpuinfo | cut -d: -f2 | xargs)
fi
echo "- **CPU**: $CPU" >> $OUTPUT
echo "" >> $OUTPUT

# 테이블 헤더 작성
echo "### Benchmark Result" >> $OUTPUT
echo "| Implementation | Tasks | Time |" >> $OUTPUT
echo "|---|---|---|" >> $OUTPUT

# 2. 컴파일 및 실행
echo "Compiling..."
g++ -std=c++20 -O3 -pthread benchmark_runner.cpp -o runner

if [ $? -eq 0 ]; then
    echo "Running Benchmark..."
    ./runner
    echo "Done."
    cat $OUTPUT
else
    echo "Compilation Failed."
fi