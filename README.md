# cpp-study

C++ 기본 문법부터 STL, Modern C++, 자료구조·알고리즘을 공부하고, 퀀트 준비용 C++ 학습까지 확장해 나가는 레포입니다.

---

## 1. 학습 목표

- C++ 전반 문법과 표준 라이브러리(STL) 이해
- 알고리즘 문제 풀이에 필요한 구현력 강화
- 이후 퀀트 준비용 C++(성능, 메모리, 스타일)로 확장

---

## 2. 진행 방식

- 하루 최소 한 문제 또는 한 개념 정리
- 각 폴더에 `README.md`로 개념 요약 정리
- 예제 코드는 필요할 때만 추가

---

## 3. 레포 구조 (계획)

- `00_real_basic` : 기본 문법
- `01_basics/` : 기본 이론
- `02_stl/` : STL 컨테이너 & 알고리즘
- `03_modern_cpp/` : Modern C++ 핵심 문법
- `04_oop/` : 클래스 / 구조체 / OOP
- `05_algorithms/` : 자료구조·알고리즘
- `99_quant_prep/` : (추후) 퀀트 준비용 C++ 정리

---

## 4. 기본 문법

- 기본 타입
- 제어문
- 함수 기초
- 참조와 포인터
- 기본 입출력

---

## 5. Modern C++ 핵심 (C++11~)

- `auto`
- range-based for
- `nullptr`
- uniform initialization
- 타입 별칭 (`using`)

---

## 6. STL 컨테이너

### 6.1 시퀀스 컨테이너
- `vector`
- `string`
- `deque`, `list` (개념 위주)

### 6.2 어댑터 컨테이너
- `stack`
- `queue`
- `priority_queue` (최대/최소 힙)

### 6.3 연관 & 해시 컨테이너
- `map`, `set`
- `unordered_map`, `unordered_set`

---

## 7. 반복자와 순회

- iterator 기본 개념
- `begin`, `end`, `rbegin`, `rend`
- 전통 for + iterator 순회
- range-based for 순회

---

## 8. <algorithm> 주요 기능

- 정렬 관련 함수
- 탐색 관련 함수
- 값/범위 관련 함수
- 누적/합계 관련 함수

---

## 9. 클래스 / 구조체 / OOP 기초

- struct / class 기본 개념
- 접근 지정자 (public / private)
- 생성자와 소멸자 개념
- 간단한 자료구조용 클래스 (ListNode, TreeNode 등)

---

## 10. 람다와 템플릿 기초

- 람다 표현식 기본 형태
- 정렬 comparator에서의 람다 활용
- 함수 템플릿 기초
- `pair`, `tuple`, 구조 분해 개념

---

## 11. Modern C++ 추가 요소 (심화)

- `constexpr` 개념
- move semantics / rvalue reference
- `emplace` 계열 함수
- `optional`, `variant`, `any` 개념

---

## 12. 메모리와 성능 감각

- 값 복사 vs 참조 전달
- `const` 사용 습관
- 주요 컨테이너 연산 시간 복잡도

---

## 13. (예정) 퀀트 준비용 C++

- 고성능 I/O 및 수치 계산
- 메모리 레이아웃과 캐시 감각
- 예외/에러 처리 전략
- 빌드 시스템 및 프로젝트 구조 (예: CMake)
- 멀티스레딩과 동시성 (필요시)

---

## 14. 참고자료

- YouTube: 코드없는 프로그래밍
- 온라인 C++ 레퍼런스
- 알고리즘 문제 풀이 사이트 (Baekjoon, LeetCode 등)
