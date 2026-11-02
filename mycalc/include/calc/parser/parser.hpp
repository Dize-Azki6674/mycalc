#pragma once

#include <calc/formula/subformula.hpp>

#include <optional>
#include <utility>
#include <vector>
#include <variant>

namespace Calc {
	
	using Token = std::variant<ValueType, BinOpr>;

	template <class T>
	concept Parser = requires (T) {
		{ T::parse(std::declval<const std::vector<Token>&>()) }
			-> std::same_as<std::optional<SubFormula>>;
	};
}