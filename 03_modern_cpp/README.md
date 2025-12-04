# 03_modern_cpp - "너 레거시 C++ 쓰는 애구나" 걸러내는 단계

quant 현직자 말: "C++11 이전 문법 쓰는 지원자는 무조건 떨어뜨린다"

## ✅ 학습 체크리스트
- [ ] shared_ptr reference counting 직접 구현
- [ ] vector<string> move vs copy 1000만개 50배 차이 확인
- [ ] lambda sort vs function_ptr 1000만개 벤치마크
- [ ] unique_ptr로 raw pointer 완전 대체
- [ ] memory_order_relaxed vs seq_cst 속도 차이 측정
- [ ] constexpr if로 컴파일타임 분기 3개 구현
- [ ] 기존 코드 50% modern C++으로 리팩토링 [web:18]


### 학습 목표
- **왜 Modern C++를 써야 하는지** 성능 숫자로 증명할 수 있게
- Smart pointer, move semantics 없으면 quant 코드 리뷰에서 바로 죽음

### C++11/14/17/20 핵심 기능 분류

| 카테고리            | 기능들                                                                 | 레거시 방식 대비 이점                                      | quant에서 필수 이유                                 |
|---------------------|------------------------------------------------------------------------|-------------------------------------------------------------|-------------------------------------------------------|
| 메모리 관리         | unique_ptr, shared_ptr, weak_ptr                                       | 수동 delete → 메모리 누수/이중 해제 100% 방지               | trading system에서 메모리 누수 = 수억 손실             |
| Move Semantics      | rvalue reference, std::move, perfect forwarding                   | 복사 대신 이동 → 대용량 데이터 10~100배 빠름                 | order book 갱신 시 복사하면 latency 폭발               |
| Lambda              | [capture](params){}                                                    | 함수 포인터보다 type erasure 없음 + 인라인 최적화 최고      | 이벤트 핸들러, comparator에 필수                      |
| constexpr           | constexpr if (C++17), consteval (C++20)                                | 컴파일 타임 계산 → 런타임 오버헤드 0                        | matching engine 규칙 컴파일 타임에 박아 넣기         |
| Concurrency         | std::thread, mutex, atomic, memory_order                               | 레거시 pthread보다 안전 + 최적화                            | multi-threaded order executor 필수                    |
| 기타 사기 기능      | auto, range-based for, structured binding, std::optional, std::string_view | 코드 길이 50% 감소 + 실수 90% 감소                          | 코드 리뷰 통과율 3배 증가                              |

### 실습 프로젝트
1. shared_ptr을 직접 구현해보기 (reference counting 포함)
2. vector<string>을 move vs copy로 1000만 번 해서 시간 측정 (move가 50배 빠름)
3. lambda vs function pointer로 sorting 1000만 개 해서 속도 비교

### quant 현실
2025년 입사자 중 100%가 unique_ptr/shared_ptr 쓰고 있음  
raw pointer 쓰는 순간 "이 애 위험하다" 낙인 찍힘

이 폴더 끝내면 너의 코드가 "현업 수준"으로 보입니다.

03_modern_cpp/
├── README.md
├── 01_smart_ptr/           # unique/shared/weak 직접 구현
│   └── smart_ptr_impl.cpp
├── 02_move_semantics/      # move vs copy 벤치마크
│   └── vector_string_bench.cpp
├── 03_lambda_opt/          # lambda vs function_ptr
│   └── sort_benchmark.cpp
├── 04_concurrency/         # atomic + memory_order
│   └── thread_test.cpp
└── modern_features/        # auto/lambda/constexpr 예제
    └── feature_demo.cpp