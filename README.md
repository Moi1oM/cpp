# cpp-study

C++ 기본 문법부터 STL, Modern C++, 자료구조·알고리즘을 공부하고, 퀀트 준비용 C++ 학습까지 확장해 나가는 레포입니다.

---

1장. C 스타일 vs Modern C++ – 왜 Modern C++을 써야 하는가?

malloc / free / realloc
raw new / delete / delete[]
void* 와 캐스팅 지옥
수동 자원 관리 (파일, 소켓, 락)
예외 안전성 완전 부재
메모리 누수 / dangling pointer / double free
zero-cost abstraction 개념
컴파일 타임 오류 검출 강화
표현력 / 가독성 / 유지보수성
Presto 같은 HFT 환경에서 C 스타일이 왜 치명적인지 (1μs의 비용)

2장. C++11/14 생산성 혁명

auto / decltype / decltype(auto)
uniform initialization (braced-init)
initializer_list
range-based for loop
constexpr (C++11 → C++14 변수/루프 허용)
if constexpr
structured bindings (C++17이지만 여기서 미리)
trailing return type
noexcept specifier
override / final
= default / = delete

3장. RAII와 스마트 포인터

RAII 원칙 (Resource Acquisition Is Initialization)
unique_ptr / make_unique
shared_ptr / weak_ptr / make_shared
enable_shared_from_this
custom deleter
std::scoped_lock (C++17, 미리 소개 가능)
rule of zero / rule of five 예고
소멸자에서 rollback / cleanup 보장
exception safety (strong / basic / no-throw)

4장. Move Semantics & Perfect Forwarding

lvalue / rvalue / xvalue / prvalue / glvalue
rvalue reference (T&&)
std::move
move constructor / move assignment
rule of five 완성
perfect forwarding
universal reference (forwarding reference)
std::forward
reference collapsing rules
noexcept move constructor 중요성 (강한 예외 보장)

5장. Lambda와 std::function

lambda expression 기본 문법
capture list ([], [=], [&], [this], init-capture C++14)
generic lambda (auto 파라미터)
mutable lambda
std::function (type erasure 비용 이해 필수)
closure type
IIFE (Immediately Invoked Lambda Expression)
std::bind (왜 이제 쓰지 말아야 하는지)
전략 패턴을 lambda로 대체하는 실전 패턴

6장. 템플릿 메타프로그래밍 실전

variadic template
fold expressions (C++17)
parameter pack
sizeof...(pack)
type traits (std::is_same_v, std::is_integral_v 등)
std::enable_if / SFINAE
concepts (C++20) ← 반드시 포함
requires expression
constrained template
if constexpr + type traits 조합
compile-time 계산 (pricing formula 등)

7장. STL 컨테이너 & 알고리즘 최적화 (low-latency 관점)

reserve / shrink_to_fit / emplace_back
small vector optimization 이해
unordered_map reserve / max_load_factor
flat_hash_map (abseil) vs unordered_map
std::string_view
std::span / gsl::span
contiguous container vs node-based
cache-friendly 순회 (SoA 미리 언급)
execution policy (seq, par, par_unseq)
std::transform / std::reduce 벡터화
allocator 개념 미리 소개

8장. Concurrency – thread, atomic, lock-free

std::thread / jthread (C++20)
std::async / std::future / std::promise
std::atomic<T> 모든 specialization
memory_order_relaxed / acquire / release / seq_cst
std::atomic wait/notify (C++20)
lock-free queue (ring buffer, MPMC)
false sharing
std::hazard_pointer (간단 소개)
transaction-safe 코드 패턴
std::mutex는 최대한 안 쓰기 (contention 비용)
core pinning / thread affinity (실전 팁)

9장. C++17/20 핵심 기능

structured bindings
if / switch init-statement
std::optional
std::variant / std::visit
std::any
std::string_view
std::span
parallel algorithms (execution::par_unseq)
std::byte
constexpr if / constexpr lambda
template <auto>
source_location (C++20)
std::jthread / std::stop_token

10장. 실전 성능 최적화 & Data-Oriented Design

cache line size (64 bytes)
false sharing 방지 (alignas(64), padding)
Structure of Arrays (SoA)
Array of Structures (AoS) vs SoA 벤치마크
alignas / alignof
custom allocator (pmr::polymorphic_allocator)
monotonic_buffer_resource / pool allocator
std::chrono::high_resolution_clock
latency 측정 패턴
branch prediction ([[likely]] / [[unlikely]])
PGO / LTO
Godbolt로 어셈블리 보는 법
Presto 실전 팁: tick-to-trade latency 1μs 이하 목표


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
