#include <calc/parser/builtin/sequential.hpp>

#include <iostream>

using namespace std;
using namespace Calc;
using namespace Calc::BuiltIn;

// 補助関数・クラス
namespace {
	void print_result(ExVal ev) {
		cout << "result: ";
		if (ev) {
			cout << *ev << "\n\n";
			return;
		}

		switch (ev.error()) {
		case Error::InvalidFormula:
			cout << "Calc Error: Invalid formula\n\n";
			return;
		case Error::Overflow:
			cout << "Calc Error: Overflow\n\n";
			return;
		case Error::DivZero:
			cout << "Calc Error: Div zero\n\n";
			return;
		}
	}

	struct TestInfo
	{
		int id = {};
		char title[32] = {};
		char expected[32] = {};

		void print() const {
			cout << "test(" << id << "): " << title << "\n";
			cout << "expected: " << expected << "\n";
		}
	};
}

// テストケース
namespace {
	// 空
	void test_empty_input() {
		TestInfo{
			.id{ 1 },
			.title{ "Empty input" },
			.expected{ "Invalid" }
		}.print();

		SeqCalc sc{};

		print_result(sc.calc());
	}

	void test_single_value_input() {
		TestInfo{
			.id{ 2 },
			.title{ "Single value input" },
			.expected{ "0" }
		}.print();

		SeqCalc sc{};
		ValueType zero{};
		sc << zero;

		print_result(sc.calc());
	}

	void test_single_operator_input() {
		TestInfo{
			.id{ 3 },
			.title{ "Single operator input" },
			.expected{ "Invalid" }
		}.print();

		SeqCalc sc{};
		auto add = BinOpr::Add;
		sc << add;

		print_result(sc.calc());
	}

	// 0割りと，ついでにoperator<<のチェインの検査
	void test_divzero_formula() {
		TestInfo{
			.id{ 4 },
			.title{ "Devide by 0" },
			.expected{ "Div zero" }
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 } << BinOpr::Div << ValueType{ 0 };

		print_result(sc.calc());
	}

	void test_dupl_operator() {
		TestInfo{
			.id{ 5 },
			.title{ "Operator duplicate" },
			.expected{ "Invalid" }
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 } << BinOpr::Div << BinOpr::Add << ValueType{ 0 };

		print_result(sc.calc());
	}

	void test_normal_formula() {
		TestInfo{
			.id{ 6 },
			.title{ "Normal formula" },
			.expected{ "29/4" }
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 }
			<< BinOpr::Add
			<< ValueType{ 2 }
			<< BinOpr::Mul
			<< ValueType{ 3 }
			<< BinOpr::Div
			<< ValueType{ 4 }
			<< BinOpr::Sub
			<< ValueType{ -5 };

		print_result(sc.calc());
	}

	void test_operator_tailed() {
		TestInfo{
			.id{7},
			.title{"Operator tailed"},
			.expected{"Invalid"}
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 } << BinOpr::Mul;
		print_result(sc.calc());
	}
	
	void test_dupl_value() {
		TestInfo{
			.id{ 8 },
			.title{ "Value duplicate" },
			.expected{ "Invalid" }
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 } << BinOpr::Mul << ValueType{} << ValueType{ 1 };
		print_result(sc.calc());
	}

	void test_error_propagate() {
		TestInfo{
			.id{ 9 },
			.title{ "Error propagation" },
			.expected{ "Div zero" }
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 }
			<< BinOpr::Mul
			<< ValueType{ 2 }
			<< BinOpr::Div
			<< ValueType{ 0 }
			<< BinOpr::Add
			<< ValueType{ 3 };
		print_result(sc.calc());
	}

	void test_result_reproduce() {
		TestInfo{
			.id{ 10 },
			.title{ "Result reproducivity" },
			.expected{ "0" }
		}.print();

		SeqCalc sc{};
		sc << ValueType{ 1 }
			<< BinOpr::Mul
			<< ValueType{};
		print_result(sc.calc());
		print_result(sc.calc());

		TestInfo{
			.id{11},
			.title{"Calc continuation"},
			.expected{"1"}
		}.print();

		sc << BinOpr::Add << ValueType{ 1 };

		print_result(sc.calc());
	}
}

int main() {
	test_empty_input();
	test_single_value_input();
	test_single_operator_input();
	test_divzero_formula();
	test_dupl_operator();
	test_normal_formula();
	test_operator_tailed();
	test_dupl_value();
	test_error_propagate();
	test_result_reproduce();

	return 0;
}