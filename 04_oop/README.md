# 04_oop - quant에서 OOP는 "필요한 만큼만"

## ✅ 학습 체크리스트
- [ ] Order 클래스 Rule of Five 완벽 구현 (복사 금지)
- [ ] CRTP로 MatchingEngine specialization 3개
- [ ] Pimpl idiom으로 컴파일 시간 50% 단축 확인
- [ ] virtual 함수 0개 코드로 다형성 구현
- [ ] 소멸자 logging RAII wrapper 완성
- [ ] final/override 키워드 모든 메서드에 적용
- [ ] latency 20ns 증가시키는 virtual 함수 찾아보기 [web:23]

quant는 OOP 신봉자가 아니라 **성능 신봉자**입니다.  
상속 깊게 쓰는 애들 탈락, composition + CRTP 쓰는 애들 합격.

### 학습 목표
- 클래스 생명주기 완벽 장악 (특히 이동 생성자/소멸자)
- virtual 함수 쓰지 않고도 다형성 구현하는 법 배우기 (CRTP)
- Rule of Five 완벽 준수

### 핵심 주제

1. **생성자/소멸자/복사/이동 완벽 이해 (Rule of Five)**
   - 이동 생성자/이동 대입 연산자 안 쓰는 순간 latency 2~5배 증가
   - 소멸자에서 자원 해제 안 하면 메모리 누수 → 실거래 시스템에서 치명적

2. **상속 vs Composition**
   - virtual 함수 = cache miss + branching → low-latency 최악
   - 대신 CRTP (Curiously Recurring Template Pattern) 사용 → 컴파일 타임 다형성, zero cost

3. **Best Practice**
   - Pimpl idiom (핵심 구현 숨기기 → 컴파일 시간 단축)
   - final 키워드 적극 사용
   - interface는 pure abstract class가 아니라 concept + template

### 실습
- Order 클래스 만들기 (복사 금지, 이동만 허용)
- MatchingEngine 기반 클래스 → CRTP로 specialization
- 소멸자에서 logging 자동으로 되는 RAII wrapper 만들기

quant 현직자 말: "virtual 쓰는 애는 바로 탈락이다. latency 20ns만 늘어도 안 됨"

04_oop/
├── README.md
├── 01_rule_of_five/        # 이동생성자/대입 완벽 구현
│   └── Order_class.cpp
├── 02_crtp/                # CRTP로 virtual 제거
│   └── MatchingEngine.cpp
├── 03_pimpl/               # 컴파일 시간 단축
│   └── PimplExample.cpp
└── raii_wrappers/          # 파일/소켓 RAII
    └── FileLogger.cpp
