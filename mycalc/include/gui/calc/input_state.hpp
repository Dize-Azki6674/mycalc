#pragma once

#include <calc/formula/subformula.hpp>

#include <boost/static_string/static_string.hpp>

#include <charconv>

template <std::size_t W>
class InputState {
public:		/* メンバ型 */
	using int_type = std::intmax_t;
	using size_type = std::size_t;
	using string_type = boost::static_string<W>;

public:		/* 定数 */
	static constexpr size_type max_digit{ W - 2 };
	// frac_size_ == nsize <-> 小数点なし
	static constexpr size_type nsize{ static_cast<size_type>(-1) };

private:	/* 内部値 */
	bool is_negative{ false };
	int_type mantissa_{};
	size_type frac_size_{ nsize };

private:	/* プライベート関数 */
	size_type total_digit_() const noexcept;

public:		/* インターフェース */
	// デフォルトコンストラクタ
	constexpr InputState() noexcept;

	// 変更
	void flip_sign() noexcept;
	void try_push_back(int n) noexcept;
	void point() noexcept;
	void clear() noexcept;

	// 型変換
	Calc::ValueType to_value() const noexcept;
	string_type to_string() const noexcept;
};

namespace {
	constexpr std::intmax_t exp10(std::size_t n) noexcept
	{
		std::intmax_t result{ 1 };

		for (std::size_t i = 0; i < n; ++i) {
			result *= 10;
		}

		return result;
	}
}

template <std::size_t W>
constexpr InputState<W>::InputState() noexcept
{
	static_assert(
		W - 2 <= std::numeric_limits<std::intmax_t>::digits10,
		"W is too large to represent"
	);
}

template <std::size_t W>
InputState<W>::size_type InputState<W>::total_digit_() const noexcept
{
	using namespace boost::static_strings;
	size_type valid_frac = (frac_size_ == nsize) ? 0 : frac_size_;
	return std::max(to_static_string(mantissa_).size(), valid_frac + 1);
}

template <std::size_t W>
void InputState<W>::flip_sign() noexcept
{
	is_negative = !is_negative;
}

template <std::size_t W>
void InputState<W>::try_push_back(int n) noexcept
{
	assert(n >= 0 && n <= 9);
	if (total_digit_() >= max_digit)
		return;

	mantissa_ = mantissa_ * 10 + n;
	if (frac_size_ != nsize)
		++frac_size_;
}

template <std::size_t W>
void InputState<W>::point() noexcept
{
	switch (frac_size_) {
	case 0:
		frac_size_ = nsize;
		return;

	case nsize:
		frac_size_ = 0;
		return;

	default:
		return;
	}
}

template <std::size_t W>
void InputState<W>::clear() noexcept
{
	is_negative = false;
	mantissa_   = 0;
	frac_size_  = nsize;
}

template <std::size_t W>
Calc::ValueType InputState<W>::to_value() const noexcept
{
	auto num = is_negative ? -mantissa_ : mantissa_;
	auto den = (frac_size_ == nsize) ? 1 : exp10(frac_size_);
	return Calc::ValueType{ num, den };
}

template <std::size_t W>
InputState<W>::string_type InputState<W>::to_string() const noexcept
{
	string_type result{};

	result.resize_and_overwrite(
		result.max_size(),
		[this](char* buf, std::size_t n) noexcept {
			const auto r = std::format_to_n(
				buf,
				n,
				"{:0{}}",
				mantissa_,
				total_digit_()
			);

			return static_cast<std::size_t>(r.out - buf);
		}
	);

	if (frac_size_ != nsize)
		result.insert(result.end() - frac_size_, '.');
	if (is_negative)
		result.insert(result.begin(), '-');

	return result;
}