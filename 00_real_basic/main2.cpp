// clang++ -std=c++20 -Wall -Wextra -O3 main.cpp -o main

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <optional>
#include <functional>

// ==================== 1. 변수 언제든 선언 가능 (C는 블록 처음에만) ====================
void demo_variable_anywhere()
{
    std::cout << "\n1. 변수 언제든 선언 (C는 블록 처음에만 가능)\n";

    int i = 0;
    std::cout << i << '\n';

    for (int j = 0; j < 3; ++j)          // j는 for 안에서만 산다 (C99도 되지만 C++이 더 철저)
    {
        std::cout << j << ' ';
        
        std::string s = "inside loop";   // 여기서 선언 가능! C는 불가능
        std::cout << s << '\n';
    }
    // std::cout << j;  // 오류! j는 이미 죽음
}

// ==================== 2. struct가 진짜 클래스처럼 됨 (멤버 함수 가능) ====================
struct Tick          // C였다면 그냥 데이터만 있었을 거야
{
    double price;
    double size;
    int64_t ts;

    // C++에서는 struct 안에 함수 넣을 수 있음 → 이게 진짜 강력
    void print() const
    {
        std::cout << "Tick{price=" << price 
                  << " size=" << size 
                  << " ts=" << ts << "}\n";
    }

    double value() const { return price * size; }
};

void demo_struct_with_methods()
{
    std::cout << "\n2. struct에 메서드 가능 (C 불가능)\n";

    Tick t{65000.0, 1.5, 1733400000000};
    t.print();
    std::cout << "value = " << t.value() << '\n';
}

// ==================== 3. 함수에 기본 인자 (default parameter) ====================
void send_order(const std::string& symbol, 
                double price, 
                double qty, 
                const std::string& side = "BUY",     // 기본값!
                const std::string& type = "LIMIT")
{
    std::cout << "Order: " << side << ' ' << qty << ' ' << symbol 
              << " @ " << price << " (" << type << ")\n";
}

void demo_default_params()
{
    std::cout << "\n3. 함수 기본 인자\n";

    send_order("BTCUSDT", 65000.0, 0.1);                    // side와 type 자동으로 BUY, LIMIT
    send_order("ETHUSDT", 3000.0, 1.0, "SELL");            // type은 여전히 LIMIT
    send_order("SOLUSDT", 150.0, 10.0, "BUY", "MARKET");   // 전부 명시
}

// ==================== 4. 강한 열거형 enum class (이름 충돌 완벽 방지) ====================
enum class Side { BUY, SELL };            // C 스타일 enum은 이름 충돌 남발
enum class OrderType { LIMIT, MARKET, STOP };

void demo_strong_enum()
{
    std::cout << "\n4. enum class (강한 열거형)\n";

    Side s = Side::BUY;
    // int x = s;          // 오류! 자동 변환 안 됨 (안전)
    int x = static_cast<int>(s);  // 명시적으로만 가능

    if (s == Side::BUY)
        std::cout << "BUY side\n";
}

// ==================== 5. auto 키워드 (타입 추론) ====================
void demo_auto()
{
    std::cout << "\n5. auto (타입 추론) - 코드가 훨씬 깔끔해짐\n";

    auto price = 65000.0;                   // double로 추론
    auto symbol = "BTCUSDT"s;               // std::string으로 추론 (접미사 s)
    auto vec = std::vector{1.1, 2.2, 3.3}; // std::vector<double>

    for (auto& v : vec)                     // v는 double&
        v *= 2;

    for (auto v : vec)
        std::cout << v << ' ';
    std::cout << '\n';
}

// ==================== 6. lambda 표현식 (익명 함수, C에 없음) ====================
void demo_lambda()
{
    std::cout << "\n6. lambda (가장 강력한 무기)\n";

    auto is_profitable = [](double entry, double current) {
        return current > entry * 1.01;      // 1% 수익 나면 true
    };

    std::cout << std::boolalpha << is_profitable(65000, 65650) << '\n'; // true

    // Quant에서 진짜 자주 쓰는 패턴
    std::vector<double> prices = {65000, 65100, 64900, 65200};
    std::sort(prices.begin(), prices.end(), [](double a, double b) {
        return a > b;                       // 내림차순
    });

    for (auto p : prices)
        std::cout << p << ' ';
    std::cout << '\n';
}

// ==================== 7. std::optional (NULL 대신 안전하게 값 없을 표현) ====================
std::optional<double> find_best_bid(const std::vector<Tick>& book)
{
    if (book.empty()) return std::nullopt;  // 값 없음
    return book[0].price;
}

void demo_optional()
{
    std::cout << "\n7. std::optional (NULL 포인터 대신 안전)\n";

    std::vector<Tick> empty_book;
    auto bid = find_best_bid(empty_book);

    if (bid.has_value())
        std::cout << "Best bid: " << bid.value() << '\n';
    else
        std::cout << "No bids\n";

    // 더 간단하게
    std::cout << "Best bid: " << (bid.value_or(0.0)) << '\n';
}

// ==================== 8. structured binding (C++17, 여러 값 한 번에 풀기) ====================
void demo_structured_binding()
{
    std::cout << "\n8. structured binding (튜플/구조체 분해)\n";

    auto get_trade = []() -> std::tuple<std::string, double, double> {
        return {"BTCUSDT", 65000.0, 0.15};
    };

    auto [symbol, price, qty] = get_trade();   // 한 줄에 세 변수로 풀림!

    std::cout << symbol << ' ' << price << ' ' << qty << '\n';
}

// ==================== main ====================
int main()
{
    demo_variable_anywhere();
    demo_struct_with_methods();
    demo_default_params();
    demo_strong_enum();
    demo_auto();
    demo_lambda();
    demo_optional();
    demo_structured_binding();

    std::cout << "\n=== 다 봤으면 이제 너는 C 개발자 → C++ 개발자 완료 ===\n";
    std::cout << "Presto Labs 코딩 테스트에서 위 기능들 다 쓰는 사람만 살아남는다.\n";
    return 0;
}