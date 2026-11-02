#pragma once

#include <boost/rational.hpp>

#include <expected>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

// 入力例：
//
//   SubFormula{ValueType{1}}
//
//   // 1 + 2
//   SubFormula{
//       std::make_unique<Add>(
//           SubFormula{ValueType{1}},
//           SubFormula{ValueType{2}}
//       )
//   }
//
//   // 1 + (2 * 3)
//   SubFormula{
//       std::make_unique<Add>(
//           SubFormula{ValueType{1}},
//           SubFormula{
//               std::make_unique<Mul>(
//                   SubFormula{ValueType{2}},
//                   SubFormula{ValueType{3}}
//               )
//           }
//       )
//   }
//
//   // (10 - 3) / 7
//   SubFormula{
//       std::make_unique<Div>(
//           SubFormula{
//               std::make_unique<Sub>(
//                   SubFormula{ValueType{10}},
//                   SubFormula{ValueType{3}}
//               )
//           },
//           SubFormula{ValueType{7}}
//       )
//   }

namespace Calc {

	enum class Error {
		Overflow,
		DivZero,
		InvalidFormula
	};
	using ValueType = boost::rational<std::intmax_t>;
	using ExVal = std::expected<ValueType, Error>;

	template <class T>
	using UPtr = std::unique_ptr<T>;

	enum class BinOpr { Add, Sub, Mul, Div };

	template <BinOpr Op>
	struct Bin;

	using Add = Bin<BinOpr::Add>;
	using Sub = Bin<BinOpr::Sub>;
	using Mul = Bin<BinOpr::Mul>;
	using Div = Bin<BinOpr::Div>;

	struct SubFormula
		: std::variant<
			std::monostate,
			ValueType,
			UPtr<Add>,
			UPtr<Sub>,
			UPtr<Mul>,
			UPtr<Div>>
	{
		constexpr ExVal evaluate() const noexcept;
	};

	template <BinOpr Op>
	struct Bin {
		SubFormula lhs{};
		SubFormula rhs{};

		static constexpr ExVal
			eval_impl(const ValueType& lhs_, const ValueType& rhs_)
			noexcept;
	};

	constexpr ExVal SubFormula::evaluate() const noexcept
	{
		return std::visit([](auto&& f) -> ExVal {
			using T = std::decay_t<decltype(f)>;

			if constexpr (std::is_same_v<T, std::monostate>)
				return std::unexpected(Error::InvalidFormula);

			else if constexpr (std::is_same_v<T, ValueType>)
				return f;

			else /* Bin */ {
				auto lhs = f->lhs.evaluate();
				if (!lhs)
					return std::unexpected(lhs.error());
				auto rhs = f->rhs.evaluate();
				if (!rhs)
					return std::unexpected(rhs.error());
				return f->eval_impl(*lhs, *rhs);
			}
		}, *this);
	}

	template <BinOpr Op>
	constexpr ExVal
		Bin<Op>::eval_impl(const ValueType& lhs_, const ValueType& rhs_)
		noexcept
	{
		if constexpr (Op == BinOpr::Add)
			return lhs_ + rhs_;
		else if constexpr (Op == BinOpr::Sub)
			return lhs_ - rhs_;
		else if constexpr (Op == BinOpr::Mul)
			return lhs_ * rhs_;
		else /* Op == BinOpr::Div */ {
			if (rhs_ == ValueType{ 0 })
				return std::unexpected(Error::DivZero);
			return lhs_ / rhs_;
		}
	}
}