# 05_algorithms - 코딩 테스트 + 실무 자료구조 구현

## ✅ 학습 체크리스트
- [ ] L3 OrderBook map<price,list<order>> 구현
- [ ] Lock-free MPMC Queue 8코어 1억 메시지 처리
- [ ] Segment Tree로 시계열 range query 구현
- [ ] 키움 OpenAPI 실시간 시세 → OrderBook 연동
- [ ] 1000만 주문 matching 1초 이내 달성
- [ ] LeetCode Hard 5문제 gp_hash_table로 30분 컷
- [ ] 코딩 테스트 패턴 10개 정리 완료 [memory:1]


quant 코딩 테스트는 LeetCode Hard + 시스템 설계 섞여 있음

### 학습 목표
- STL로 LeetCode Hard 30분 컷
- OrderBook, Matching Engine 직접 구현 가능하게

### 필수 구현 목록

1. **Order Book** (L3 지원, 가격-시간 우선순위)
   - map<price, list<order>> + map<order_id, iterator>로 O(log n) 삽입/취소
   - 또는 vector + 정렬로 구현해서 속도 비교

2. **Lock-free Queue** (Michael-Scott 또는 MPMC)
   - std::atomic으로 구현
   - 8코어에서 1억 메시지 처리 속도 측정

3. **Graph/Tree**
   - Trie (order id prefix search용)
   - Segment Tree (시계열 데이터 range query용)
   - Union-Find with path compression + rank

4. **코딩 테스트 패턴**
   - Sliding Window → deque
   - Two Pointer → sort + vector
   - DP → vector + memoization (unordered_map 금지 → gp_hash_table)

### 실습 프로젝트
- 키움 Open API로 실시간 시세 받아서 Order Book 구현
- 1000만 주문 넣고 matching 시간 측정 (목표: 1초 이내)

이 폴더 끝내면 quant 코딩 테스트 95점 이상 나옵니다.

05_algorithms/
├── README.md
├── 01_orderbook/           # L3 Order Book 구현
│   ├── l3_orderbook.cpp
│   └── kiwoom_demo.cpp
├── 02_lockfree_queue/      # Michael-Scott Queue
│   └── mpmc_queue.cpp
├── 03_data_structures/     # Trie/SegmentTree/UnionFind
│   └── advanced_ds.cpp
└── leetcode_hard/          # STL+gp_hash_table 풀이
    └── hard_problems.cpp
