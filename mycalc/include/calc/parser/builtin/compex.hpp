#pragma once

#include <calc/calc.hpp>
#include <calc/parser/parser.hpp>

#include <optional>
#include <vector>

namespace Calc {
	struct CompEx;	// 宣言のみ

	namespace BuiltIn {
		using CompCalc = Calculator<CompEx>;
	}
}