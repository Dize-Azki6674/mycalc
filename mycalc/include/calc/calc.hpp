#pragma once

#include <calc/parser/parser.hpp>

#include <optional>
#include <vector>
#include <variant>

namespace Calc {

	template <Parser P>
	class Calculator
	{
		std::vector<Token> tokens_{};

	public:
		// 実行
		ExVal calc() const noexcept;

		// リセット
		void reset() noexcept;

		// 入力
		Calculator<P>& operator<<(Token token) noexcept;
	};

	template <Parser P>
	ExVal Calculator<P>::calc() const noexcept
	{
		auto parse_res = P::parse(tokens_);
		if (!parse_res)
			return std::unexpected(Error::InvalidFormula);

		return parse_res->evaluate();
	}

	template <Parser P>
	void Calculator<P>::reset() noexcept
	{
		tokens_.clear();
	}

	template <Parser P>
	Calculator<P>& Calculator<P>::operator<<(Token token) noexcept
	{
		tokens_.push_back(token);
		return *this;
	}

}