# 02_stl - quant가 "너 STL 제대로 아는 애구나" 깨닫는 순간

## ✅ 학습 체크리스트
- [ ] vector capacity 1.5x vs 2x 리사이징 벤치마크
- [ ] gp_hash_table 헤더 포함 + LeetCode 1문제 교체
- [ ] 모든 컨테이너 iterator invalidation 조건 암기
- [ ] list splice() 100만개로 vector insert 압살 확인
- [ ] reserve() 없이 vector 100만개 넣고 시간 측정
- [ ] policy_based_data_structure 완벽 컴파일 통과
- [ ] LeetCode Medium 3문제 gp_hash_table로 10분 컷 [memory:1]

quant 코딩 테스트 Hard 문제는 거의 다 STL로 10배 빠르게 풀 수 있습니다.
하지만 "vector가 리사이징할 때 iterator 무효화되는 거 아나요?" 한 방에 걸러냅니다.

### 학습 목표
- 모든 컨테이너의 **내부 구현 방식 + 시간 복잡도 + 메모리 구조** 완벽 암기
- **Iterator invalidation 조건**을 눈 감고도 말할 수 있게
- quant 코딩 테스트에서 **unordered_map 대신 gp_hash_table 쓰는 수준** 되기

### Sequence Containers
| 컨테이너       | 내부 구조           | 삽입/삭제 (중간) | 접근 | 메모리 특징              | Iterator 무효화 조건                          |
|----------------|---------------------|------------------|-------------------|------|--------------------------|-----------------------------------------------|
| vector         | 연속 메모리         | O(n)             | O(1) | Cache-friendly 최고     | capacity 변경 시 모든 iterator 무효화         |
| deque          | chunk 배열          | O(1) (양 끝)     | O(1) | Cache-friendly 중       | 중간 삽입/삭제 시 모든 iterator 무효화        |
| list           | 이중 연결 리스트    | O(1)             | O(n) | Cache 최악              | 삽입/삭제한 노드만 무효화                     |

### Associative Containers
| 컨테이너              | 내부 구조              | 평균 시간 복잡도 | 최악 | 메모리 오버헤드 | Iterator 무효화 |
|-----------------------|------------------------|------------------|------|------------------|-----------------|
| set/map               | Red-Black Tree         | O(log n)        | O(log n) | 높음            | 삽입/삭제 시 유효 |
| unordered_set/map     | 해시 테이블 (체이닝)   | O(1)             | O(n) | 중간            | rehash 시 모든 iterator 무효화 |
| gp_hash_table (추천)  | Open Addressing        | O(1)             | O(n) | 낮음            | rehash 시 모든 무효화 |

### Container Adapters
- stack/queue/priority_queue → 내부 컨테이너 바꿔쓰기 가능 (deque가 디폴트)

### 실습 필수
1. vector 리사이징 시 capacity 1.5배 vs 2배 속도 비교
2. unordered_map load factor 0.7 vs 0.9 vs 2.0 성능 측정
3. list splice vs vector insert 100만 번 비교 (list가 100배 빠름)

### quant 꿀팁
- 코딩 테스트에서 unordered_map 쓰지 말고 **policy_based_data_structure**의 gp_hash_table 쓰세요 (충돌 거의 없음)
- reserve() 안 쓰는 순간 바로 탈락각

이 폴더 끝내면 LeetCode Hard 70%가 STL로 15분 컷 됩니다.

02_stl/
├── README.md
├── 01_vector_deepdive/     # 리사이징 + iterator invalidation
│   └── vector_test.cpp
├── 02_hash_tables/          # gp_hash_table vs unordered_map
│   ├── policy_based.cpp
│   └── load_factor_bench.cpp
├── 03_list_splice/          # list vs vector 중간 삽입
│   └── splice_bench.cpp
└── container_benchmarks/    # 모든 컨테이너 성능 비교
    └── all_containers.cpp
