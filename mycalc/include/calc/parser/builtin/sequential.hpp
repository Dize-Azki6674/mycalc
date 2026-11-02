#pragma once

#include <calc/calc.hpp>
#include <calc/parser/parser.hpp>

#include <optional>
#include <vector>

namespace Calc {

	struct Sequential {
		static std::optional<SubFormula>
			parse(const std::vector<Token>& tokens);
	};

	namespace BuiltIn {
		using SeqCalc = Calculator<Sequential>;
	};
}