// main.cpp - 한 파일에 다 때려박고 한 번에 돌려보자
// 컴파일: clang++ -std=c++20 -Wall -Wextra -O3 main.cpp -o main

#include <iostream>
#include <string>
#include <vector>
#include <memory>   // 스마트 포인터 (나중에 쓸 거지만 미리 include)

// 1. 참조 (&) — C의 포인터를 완전히 대체하는 안전한 방법
void demo_reference()
{
    std::cout << "\n=== 1. 참조 (Reference) ===\n";

    int x = 10;
    int& ref = x;        // ref는 x의 또 다른 이름 (별명)
    ref = 20;            // x도 20이 된다!

    std::cout << "x = " << x << ", ref = " << ref << '\n'; // 20 20

    // const 참조 — 함수 인자로 큰 객체 넘길 때 무조건 이거 써라 (복사 방지)
    std::string s = "Hello World";
    const std::string& cref = s;  // 복사 안 함, 수정도 못 함
    std::cout << cref << '\n';

    // C였다면 이렇게 썼을 거야 (절대 이렇게 쓰지 마!)
    // std::string* ptr = &s;
    // std::cout << *ptr << '\n';
}

// 2. 함수 오버로딩 (Function Overloading)
void print(int i)                  { std::cout << "int: " << i << '\n'; }
void print(double d)               { std::cout << "double: " << d << '\n'; }
void print(const std::string& s)   { std::cout << "string: " << s << '\n'; }
void print(const char* s)          { std::cout << "C-string: " << s << '\n'; }

void demo_overloading()
{
    std::cout << "\n=== 2. 함수 오버로딩 ===\n";
    print(42);
    print(3.14);
    print("Hello");               // const char*
    print(std::string("World"));  // std::string
}

// 3. namespace — 이름 충돌 방지 (C의 static 전역 함수 같은 역할)
namespace my_company
{
    namespace trading
    {
        void start() { std::cout << "my_company::trading::start()\n"; }
    }

    namespace utils
    {
        void log(const std::string& msg) { std::cout << "[LOG] " << msg << '\n'; }
    }
}

void demo_namespace()
{
    std::cout << "\n=== 3. namespace ===\n";
    my_company::trading::start();
    my_company::utils::log("System started");

    // 자주 쓰는 건 using으로 줄여라
    using my_company::utils::log;
    log("이제 이렇게 짧게 쓸 수 있음");
}

// 4. 클래스 기본 (public/private, constructor/destructor)
class OrderBook
{
private:                          // 외부에서 절대 접근 불가
    std::string symbol;
    double best_bid;
    double best_ask;

public:
    // 생성자 (Constructor) — 3가지 스타일 다 보여줌
    OrderBook() = default;        // 기본 생성자 (컴파일러가 만들어줌)

    explicit OrderBook(const std::string& sym)   // explicit: 암시적 변환 금지 (좋은 습관)
        : symbol(sym), best_bid(0.0), best_ask(0.0)
    {
        std::cout << symbol << " OrderBook created\n";
    }

    // 복사 금지 (Quant 코드에선 거의 항상 이렇게 함)
    OrderBook(const OrderBook&) = delete;
    OrderBook& operator=(const OrderBook&) = delete;

    // 이동 생성자 & 이동 대입 (Modern C++ 필수)
    OrderBook(OrderBook&& other) noexcept
        : symbol(std::move(other.symbol))
        , best_bid(other.best_bid)
        , best_ask(other.best_ask)
    {
        std::cout << "OrderBook moved!\n";
    }

    OrderBook& operator=(OrderBook&& other) noexcept
    {
        if (this != &other)
        {
            symbol = std::move(other.symbol);
            best_bid = other.best_bid;
            best_ask = other.best_ask;
        }
        return *this;
    }

    // 소멸자 (Destructor)
    ~OrderBook()
    {
        std::cout << symbol << " OrderBook destroyed\n";
    }

    // public 메서드
    void update(double bid, double ask)
    {
        best_bid = bid;
        best_ask = ask;
    }

    void print() const               // const 멤버 함수 (상태 안 바꿈)
    {
        std::cout << symbol << " bid=" << best_bid
                  << " ask=" << best_ask << '\n';
    }
};

void demo_class()
{
    std::cout << "\n=== 4. 클래스 기본 ===\n";

    OrderBook ob1("BTCUSDT");
    ob1.update(65000.0, 65001.0);
    ob1.print();

    // 이동 테스트
    OrderBook ob2 = std::move(ob1);   // move constructor 호출
    ob2.print();
    // ob1은 이제 비어있음 (moved-from 상태)
}

// 5. std::cout, std::string, std::vector 실전 예제
void demo_stl_basics()
{
    std::cout << "\n=== 5. std::cout / string / vector ===\n";

    // std::string — C의 char* 대신 이거만 써라
    std::string s1 = "BTC";
    std::string s2 = "USDT";
    std::string symbol = s1 + s2;               // 연결 가능
    symbol += "_PERP";                          // += 가능
    std::cout << symbol << '\n';                // BTCUSDT_PERP

    // std::vector — 동적 배열, C의 realloc 필요 없음
    std::vector<double> prices = {65000.0, 65001.5, 64999.0, 65002.0};

    prices.push_back(65003.5);                  // 끝에 추가 (자동으로 크기 늘림)
    prices.emplace_back(65004.0);               // 더 빠름 (직접 생성)

    std::cout << "prices size = " << prices.size() << '\n';

    for (double p : prices)                      // range-based for (C++11~)
    {
        std::cout << p << ' ';
    }
    std::cout << '\n';

    // vector 안의 vector (Quant에서 매우 자주 씀)
    std::vector<std::vector<double>> orderbook = {
        {65004.0, 1.5},    // price, quantity
        {65003.0, 2.3},
        {65002.0, 0.8}
    };

    for (const auto& level : orderbook)
    {
        std::cout << "price=" << level[0] << " qty=" << level[1] << '\n';
    }
}

// main 함수
int main()
{
    demo_reference();
    demo_overloading();
    demo_namespace();
    demo_class();
    demo_stl_basics();

    // vector에 OrderBook 넣어보기 (실제 Quant 코드 스타일)
    std::vector<OrderBook> books;
    books.emplace_back("ETHUSDT");
    books.emplace_back("SOLUSDT");

    for (auto& book : books)
    {
        book.update(3000.0, 3001.0);
        book.print();
    }

    std::cout << "\nProgram end.\n";
    return 0;
}