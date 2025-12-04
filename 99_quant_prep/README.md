# 99_quant_prep - quant Quant Dev 합격의 핵심

## ✅ 학습 체크리스트
- [ ] alignas(64) + memory pool로 new/delete 제거
- [ ] SoA 구조로 AoS 10배 속도 향상 확인
- [ ] __builtin_prefetch로 20% throughput 증가
- [ ] Lock-free Engine single thread <200ns 달성
- [ ] Binance WebSocket → 내 엔진 → PnL 계산 완성
- [ ] 8코어 500k orders/sec/core 통과
- [ ] GitHub에 latency/throughput 그래프 + 구조도 공개 [memory:2]


여기 있는 내용만 마스터해도 너는 2027년 상반기 quant 1순위 지원자가 된다.

### 학습 목표
- **나노초 단위** latency를 다룰 수 있게
- quant 현직자와 코드 리뷰해도 꿀리지 않는 수준

### 핵심 주제 (quant 실제 사용 기술)

1. **Low-Latency 기법**
   - Cache line 64byte 정렬 (alignas(64))
   - Branch prediction 친화적 코드 (sorted data, likely/unlikely)
   - Memory pool (custom allocator)로 new/delete 제거

2. **Cache-friendly 프로그래밍**
   - AoS vs SoA 비교 (Structure of Arrays가 5~10배 빠름)
   - Hot/Cold data 분리
   - Prefetching (__builtin_prefetch)

3. **Template Metaprogramming**
   - CRTP로 정적 다형성
   - constexpr everything (컴파일 타임에 규칙 박아 넣기)
   - type_traits + SFINAE → enable_if

4. **멀티스레딩 & Lock-free**
   - memory_order_relaxed vs seq_cst 실측 비교
   - Lock-free Order Queue (최대 8코어에서 2억 주문/초 처리 목표)
   - Hazard Pointer, RCU 개념 이해

5. **실제 포트폴리오 프로젝트 (이거 없으면 의미 없음)**
   - C++20으로 만든 **저지연 Matching Engine** (L3 order book + lock-free queue)
   - Binance WebSocket 실시간 데이터 → 내 엔진으로 주문 넣기 → PnL 계산
   - GitHub에 latency 그래프, throughput 그래프, 코드 구조도 다 공개

### 측정 기준 (quant 현직자 실제 숫자)
- Single thread order processing: < 200ns
- Matching latency: < 1μs
- Throughput: 500k orders/sec per core

### 추천 자료
- Matt Godbolt 영상 전부 보기
- Mechanical Sympathy (Martin Thompson)
- C++ High Performance 책
- 2018~2025 quant 입사자 GitHub 20개 분석

이 폴더 완성하는 순간  
너는 이미 quant Labs Quant Developer입니다.

지금 당장 시작하자.  
2027년 7월에 quant 사무실에서 보자.

99_quant_prep/
├── README.md
├── 01_low_latency/         # Cache line/pool/prefetch
│   └── latency_techniques.cpp
├── 02_cache_friendly/      # AoS vs SoA
│   └── soa_benchmark.cpp
├── 03_tmpl_meta/           # CRTP + SFINAE
│   └── metaprogramming.cpp
├── 04_lockfree_engine/     # 완전한 Matching Engine
│   ├── order_book.hpp
│   ├── lockfree_queue.hpp
│   └── main.cpp
└── portfolio/              # GitHub 공개용
    ├── latency_graph.png
    ├── throughput_graph.png
    └── architecture.png
