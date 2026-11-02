#include <gui/calc/manager.hpp>

#include <array>
#include <iostream>
#include <string_view>
#include <initializer_list>

// テスト用
namespace {
    using namespace std;
    using namespace Calc;

    struct TestInfo {
        int id{};
        string_view title{};
        string_view expected{};

        void print() const {
            cout << "test(" << id << "): " << title << '\n';
            cout << "expected: [" << expected << "]\n";
        }
    };

    int test_count{};
    int failure_count{};

    void test(
        string_view title,
        initializer_list<CalcEvent> events,
        string_view expected)
    {
        ++test_count;

        Manager manager{};
        for (const auto& event : events) {
            manager.on_event(event);
        }

        const auto actual = manager.format();

        cout << "test(" << test_count << "): " << title << '\n';
        cout << "expected: [" << expected << "]\n";
        cout << "actual:   [" << actual << "]\n";

        if (actual != expected) {
            ++failure_count;
            cout << "FAILED\n";
        }
        else {
            cout << "OK\n";
        }

        cout << '\n';
    }
}

// 数字入力
void test_number_input() {
    using namespace std;

    test(
        "Single digit",
        { Number{ 1 } },
        "1");

    test(
        "Multiple digits",
        { Number{ 1 }, Number{ 2 }, Number{ 3 } },
        "123");

    test(
        "Zero input",
        { Number{ 0 } },
        "0");

    test(
        "Leading zero",
        { Number{ 0 }, Number{ 0 }, Number{ 1 } },
        "1");

    test(
        "Maximum digit count",
        {
            Number{ 1 }, Number{ 2 }, Number{ 3 },
            Number{ 4 }, Number{ 5 }, Number{ 6 },
            Number{ 7 }, Number{ 8 }
        },
        "12345678");

    test(
        "Input beyond maximum digit count",
        {
            Number{ 1 }, Number{ 2 }, Number{ 3 },
            Number{ 4 }, Number{ 5 }, Number{ 6 },
            Number{ 7 }, Number{ 8 }, Number{ 9 }
        },
        "12345678");
}

// 小数点・符号
void test_number_format() {
    test(
        "Decimal point",
        { Number{ 1 }, Point{}, Number{ 2 }, Number{ 5 } },
        "1.25");

    test(
        "Trailing decimal point",
        { Number{ 1 }, Point{} },
        "1.");

    test(
        "Trailing zero",
        { Number{ 1 }, Point{}, Number{ 0 } },
        "1.0");

    test(
        "Negative number",
        { Sign{}, Number{ 1 }, Number{ 2 } },
        "-12");

    test(
        "Negative decimal",
        { Sign{}, Number{ 1 }, Point{}, Number{ 5 } },
        "-1.5");
}

// 入力のクリア
void test_clear() {
    test(
        "C clears input",
        { Number{ 1 }, Number{ 2 }, C{} },
        "0");

    test(
        "AC clears input",
        { Number{ 1 }, Number{ 2 }, AC{} },
        "0");

    test(
        "C after decimal input",
        { Number{ 1 }, Point{}, Number{ 2 }, C{} },
        "0");

    test(
        "C after sign input",
        { Sign{}, Number{ 5 }, C{} },
        "0");
}

// 四則演算
void test_calculation() {
    test(
        "Addition",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Number{ 2 }, Equal{}
        },
        "3");

    test(
        "Subtraction",
        {
            Number{ 5 }, Binary{ BinOpr::Sub },
            Number{ 2 }, Equal{}
        },
        "3");

    test(
        "Multiplication",
        {
            Number{ 3 }, Binary{ BinOpr::Mul },
            Number{ 4 }, Equal{}
        },
        "12");

    test(
        "Division",
        {
            Number{ 8 }, Binary{ BinOpr::Div },
            Number{ 2 }, Equal{}
        },
        "4");

    test(
        "Fraction result",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 2 }, Equal{}
        },
        "0.5");

    test(
        "Decimal calculation",
        {
            Number{ 1 }, Point{}, Number{ 5 },
            Binary{ BinOpr::Add },
            Number{ 2 }, Point{}, Number{ 5 },
            Equal{}
        },
        "4");
}

// イベント列の連続操作
void test_state_transitions() {
    test(
        "Result followed by number",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Number{ 2 }, Equal{},
            Number{ 5 }
        },
        "5");

    test(
        "Result followed by decimal input",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Number{ 2 }, Equal{},
            Point{}, Number{ 5 }
        },
        "0.5");

    test(
        "Result followed by AC",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Number{ 2 }, Equal{},
            AC{}
        },
        "0");

    test(
        "Result followed by C",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Number{ 2 }, Equal{},
            C{}
        },
        "0");

    test(
        "Consecutive binary operators",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Binary{ BinOpr::Mul },
            Number{ 2 }, Equal{}
        },
        "2");

    test(
        "Repeated equal",
        {
            Number{ 1 }, Binary{ BinOpr::Add },
            Number{ 2 }, Equal{}, Equal{}
        },
        "3");
}

// エラー処理
void test_errors() {
    test(
        "Division by zero",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 0 }, Equal{}
        },
        "Div0!");

    test(
        "AC after division by zero",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 0 }, Equal{}, AC{}
        },
        "0");
}

void test_input_after_errors() {
    test(
        "Number input after divzero",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 0 }, Equal{}, Number{ 1 }
        },
        "1");

    test(
        "Sign input after divzero",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 0 }, Equal{}, Sign{}
        },
        "-0");

    test(
        "Point input after divzero",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 0 }, Equal{}, Point{}
        },
        "0.");
}


void test_fraction_format() {
    // 無限小数
    test(
        "One third",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 3 }, Equal{}
        },
        "0.3333333");

    test(
        "Two thirds",
        {
            Number{ 2 }, Binary{ BinOpr::Div },
            Number{ 3 }, Equal{}
        },
        "0.6666667");

    test(
        "One sixth",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 6 }, Equal{}
        },
        "0.1666667");

    test(
        "One seventh",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 7 }, Equal{}
        },
        "0.1428571");

    // 整数部がある無限小数
    test(
        "Ten thirds",
        {
            Number{ 1 }, Number{ 0 },
            Binary{ BinOpr::Div },
            Number{ 3 }, Equal{}
        },
        "3.3333333");

    test(
        "One hundred thirds",
        {
            Number{ 1 }, Number{ 0 }, Number{ 0 },
            Binary{ BinOpr::Div },
            Number{ 3 }, Equal{}
        },
        "33.333333");

    // 負の無限小数
    test(
        "Negative one third",
        {
            Sign{}, Number{ 1 },
            Binary{ BinOpr::Div },
            Number{ 3 }, Equal{}
        },
        "-0.3333333");

    // 有限小数
    test(
        "One eighth",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 8 }, Equal{}
        },
        "0.125");

    test(
        "One twentieth",
        {
            Number{ 1 }, Binary{ BinOpr::Div },
            Number{ 2 }, Number{ 0 }, Equal{}
        },
        "0.05");

    // 整数になる除算
    test(
        "Six divided by two",
        {
            Number{ 6 }, Binary{ BinOpr::Div },
            Number{ 2 }, Equal{}
        },
        "3");
}


void test_max_value_format() {
    // 整数部が最大桁数
    test(
        "Maximum integer value",
        {
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Equal{}
        },
        "99999999");

    // 最大値付近の小数
    test(
        "Maximum threshold",
        {
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Point{},
            Number{ 5 }, Equal{}
        },
        "9999999.5");

    // 最大値を超える値を計算
    test(
        "Above maximum threshold",
        {
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Binary{ BinOpr::Add },
            Number{ 1 }, Equal{}
        },
        "########");

    // 小数部を持つ大きな値の除算
    test(
        "Large value with recurring decimal",
        {
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Binary{ BinOpr::Div },
            Number{ 7 }, Equal{}
        },
        "14285714");

    // 最大値付近では四捨五入でなく切り捨てになる（仕様）
    test(
        "Large value with recurring decimal",
        {
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 7 },
            Binary{ BinOpr::Div },
            Number{ 7 }, Equal{}
        },
        "14285713");

    // 表示幅に収まる小数桁数の確認
    test(
        "Large value with decimal part",
        {
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Number{ 9 },
            Number{ 9 }, Number{ 9 }, Number{ 9 }, Point{},
            Number{ 9 },
            Binary{ BinOpr::Div },
            Number{ 3 }, Equal{}
        },
        "3333333.3");
}

void test_repeated_decimal_addition() {
    test(
        "Repeated decimal addition",
        {
            Number{ 0 }, Point{}, Number{ 1 },
            Binary{ BinOpr::Add },
            Number{ 0 }, Point{}, Number{ 1 },
            Binary{ BinOpr::Add },
            Number{ 0 }, Point{}, Number{ 1 },
            Equal{}
        },
        "0.3");
}

int main() {
    test_number_input();
    test_number_format();
    test_clear();
    test_calculation();
    test_state_transitions();
    test_errors();
    test_input_after_errors();
    test_fraction_format();
    test_max_value_format();
    test_repeated_decimal_addition();

    std::cout
        << "Tests: " << test_count
        << ", Failures: " << failure_count << '\n';

    return failure_count == 0 ? 0 : 1;
}
