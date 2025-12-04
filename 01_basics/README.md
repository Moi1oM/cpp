# 01_basics - 진짜 기초가 아니라 "quant가 죽어라 보는 핵심 기초"

## ✅ 학습 체크리스트
- [ ] 메모리 레이아웃 5개 영역 그림 외우기
- [ ] `int*`, `int&`, `int&&` Godbolt 비교 완료
- [ ] 스택 1000만 vs 힙 1000만 할당 벤치마크 돌려보기
- [ ] Stack overflow 재귀로 직접 발생시켜보기
- [ ] MemoryTester 프로젝트 완성 + Valgrind clean
- [ ] RAII 파일로거 만들어서 자동 flush 확인

quant 면접관이 제일 먼저 물어보는 게 90%가 여기서 나옵니다.
"스택에 뭐가 올라가죠?" "힙 할당은 누가 해제하나요?" "포인터 vs 참조 차이 정확히 말해보세요" "RAII가 뭔가요?"

### 학습 목표
- 메모리 모델을 완전히 이해해서 **어떤 변수가 어디에 있는지 눈 감고도 말할 수 있게**
- 포인터/참조를 자유자재로 다뤄서 **나중에 smart pointer/move semantics 이해의 기반** 만들기
- quant 코딩 테스트에서 **메모리 관련 버그 0개** 만들기

### 필수 이해 주제 (순서대로 정복)

1. **메모리 레이아웃 완전 정복**
   - Text / Data / BSS / Heap / Stack 정확한 위치와 특징
   - `sizeof`가 언제 컴파일 타임에 결정되는지 런타임에 결정되는지
   - Stack overflow 실제로 일으켜보기 (재귀 100만 번)

2. **포인터 vs 참조 철저 비교**
   - 참조는 사실 상수 포인터라는 것 증명해보기
   - `int* p`, `int& r`, `int&& rr` 세 개의 어셈블리 코드 비교 (Godbolt)
   - 참조가 재지정 불가능한 이유 → 컴파일러 최적화 차이

3. **스택 vs 힙 실생활 차이**
   - 스택 할당 1000만 개 vs 힙 할당 1000만 개 속도 측정 (실제 코드 돌려보기)
   - malloc/free vs new/delete 차이 (malloc은 constructor 안 부름!)
   - 메모리 단편화 실제로 일으켜보기 (힙에 1GB 반복 할당/해제)

4. **생성자/소멸자 호출 타이밍 완벽 이해 (RAII 기반)**
   - 객체가 스택에 있을 때 vs 힙에 있을 때 소멸자 호출 시점 차이
   - 예외 발생 시 스택 unwinding 과정 직접 디버깅해보기
   - `std::vector` 리사이징 시 기존 객체들 소멸자/생성자 호출 여부 확인

### 실습 프로젝트 (이거 안 하면 의미 없음)
- `MemoryTester` 클래스 만들어서
- 스택/힙 할당 시간 측정
- 메모리 누수 의도적으로 만들고 Valgrind로 잡기
- RAII 기반 파일 로거 만들기 (소멸자에서 자동 flush)

### 추천 자료
- Godbolt.org (매일 들어가기)
- Valgrind + GDB 필수 설치
- 책: Effective Modern C++ Item 1~4 다시 읽기

이 폴더 끝내면 quant 전화면접 메모리 질문 100% 통과합니다.

01_basics/
├── README.md
├── 01_memory_layout/        # Text/Data/BSS/Heap/Stack
│   ├── main.cpp
│   └── memory_diagram.png
├── 02_pointer_ref/          # 포인터 vs 참조 어셈블리
│   ├── pointer.cpp
│   ├── reference.cpp
│   └── godbolt_links.md
├── 03_stack_heap_bench/     # 할당 속도 비교
│   └── benchmark.cpp
├── 04_raii_demo/            # 생성자/소멸자 타이밍
│   └── raii_test.cpp
└── MemoryTester/            # 실습 프로젝트
    ├── src/
    ├── tests/
    └── CMakeLists.txt
