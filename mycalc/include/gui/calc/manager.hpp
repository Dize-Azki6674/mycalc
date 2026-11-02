#pragma once

#include <calc/parser/builtin/sequential.hpp>

#include <gui/calc/event.hpp>
#include <gui/calc/input_state.hpp>

#include <boost/static_string/static_string.hpp>

#include <optional>

class Manager {
public:
	using size_type = std::size_t;
	using value_type = Calc::ValueType;
	using int_type = value_type::int_type;

	static constexpr size_type disp_width{ 10 };
	using string_type = boost::static_string<disp_width>;
	using input_type = InputState<disp_width>;
	using operator_enum = Calc::BinOpr;

	static constexpr size_type max_digit{ disp_width - 2 };
	static constexpr value_type max_th{ 200000000 - 1, 2 };	// 99999999.5

	struct DispInfo {
		string_type value{};
		std::optional<operator_enum> opr{};
	};

private:
	input_type innum_{};
	std::optional<Calc::BinOpr> binop_{};
	Calc::ExVal aggregate_{};

	Calc::BuiltIn::SeqCalc sc_;

	enum class Mode { Input, Result } mode_{};

	struct EventVisitor {
		Manager& manager;
		void operator()(Number num) noexcept;
		void operator()(Sign) noexcept;
		void operator()(Point) noexcept;
		void operator()(Binary bin) noexcept;
		void operator()(Equal) noexcept;
		void operator()(AC) noexcept;
		void operator()(C) noexcept;
	};

public:
	Manager() noexcept;

	void on_event(const CalcEvent e) noexcept;

	// 0.1 -> 0.1, 0.10000001 -> 0.1000000で表記したい
	// aggregate * 10^n で aggregate.den != 1 となる桁 n を利用できそう
	string_type format() const noexcept;

	DispInfo dispinfo() const noexcept;

private:
	// ビジター内で使う補助関数
	// メンバ変数をすべてリセット
	void reset_() noexcept;
	// モードをInputに変更し，innum_への参照を返す
	input_type& begin_input_() noexcept;
	// Mode::Inputなら計算して結果を更新
	void commit_() noexcept;

	// format_value_中で使う補助関数
	static int solve_exp_(int_type& x, const int_type b) noexcept;
	static int prec_(const value_type& x, int maxp) noexcept;

	string_type format_error_() const noexcept;
	string_type format_value_(value_type value) const noexcept;
	string_type format_aggregate_() const noexcept;
};