#include <gui/calc/event.hpp>
#include <gui/calc/manager.hpp>

#include <array>
#include <variant>

void Manager::EventVisitor::operator()(Number num) noexcept
{
	manager.begin_input_().try_push_back(num.value);
}

void Manager::EventVisitor::operator()(Sign) noexcept
{
	manager.begin_input_().flip_sign();
}

void Manager::EventVisitor::operator()(Point) noexcept
{
	manager.begin_input_().point();
}

void Manager::EventVisitor::operator()(Binary bin) noexcept
{
	manager.commit_();
	manager.binop_ = bin;
}

void Manager::EventVisitor::operator()(Equal) noexcept
{
	manager.commit_();
	manager.binop_.reset();
}

void Manager::EventVisitor::operator()(AC) noexcept
{
	manager.reset_();
}

void Manager::EventVisitor::operator()(C) noexcept
{
	if (manager.mode_ == Manager::Mode::Result)
		manager.reset_();
	else
		manager.begin_input_().clear();
}

Manager::Manager() noexcept
{
	sc_ << value_type{};
}

void Manager::on_event(const CalcEvent e) noexcept
{
	std::visit(EventVisitor{ *this }, e);
}

Manager::string_type Manager::format() const noexcept
{
	if (mode_ == Mode::Input)
		return innum_.to_string();
	else
		return format_aggregate_();
}

Manager::DispInfo Manager::dispinfo() const noexcept
{
	return DispInfo{
		.value{ format() },
		.opr{ binop_ }
	};
}

void Manager::reset_() noexcept
{
	mode_ = Mode::Input;
	sc_.reset();
	sc_ << value_type{};
	binop_.reset();
	innum_.clear();
}

Manager::input_type& Manager::begin_input_() noexcept
{
	mode_ = Mode::Input;
	return innum_;
}

void Manager::commit_() noexcept {
	if (mode_ == Mode::Result)
		return;

	auto input_val = innum_.to_value();
	mode_ = Mode::Result;
	innum_.clear();
	if (binop_ && aggregate_)
		sc_ << *binop_;
	else
		sc_.reset();

	sc_ << input_val;
	aggregate_ = sc_.calc();
}


int Manager::solve_exp_(int_type& x, const int_type b) noexcept
{
	int exp = 0;
	while (x % b == 0) {
		x /= b;
		exp++;
	}
	return exp;
}

int Manager::prec_(const value_type& x, int maxp) noexcept
{
	auto den = x.denominator();
	auto e2 = solve_exp_(den, 2);
	auto e5 = solve_exp_(den, 5);
	if (den == 1)
		return std::min( std::max(e2, e5), maxp );
	else
		return maxp;
}

Manager::string_type Manager::format_error_() const noexcept
{
	switch (aggregate_.error()) {
	case Calc::Error::DivZero:
		return "Div0!";
	case Calc::Error::Overflow:
		return "########";
	case Calc::Error::InvalidFormula:
		return "panic!";
	}
}

Manager::string_type Manager::format_value_(value_type value) const noexcept
{
	auto dbl_val = boost::rational_cast<double>(value);
	std::array<char, 2 * disp_width> buf{};

	// 一旦固定精度で変換
	static_assert(std::in_range<int>(max_digit - 1));
	const auto result = std::to_chars(
		buf.data(),
		buf.data() + buf.size(),
		dbl_val,
		std::chars_format::fixed,
		prec_(value, static_cast<int>(max_digit - 1))
	);

	// 最大幅と固定精度で変換した結果の長さを比較し，超過分を切り詰め
	const auto len = static_cast<std::size_t>(result.ptr - buf.data());
	const auto sgn_adjuster = static_cast<std::size_t>(value >= 0);
	const auto max_width_acount_sign = disp_width - sgn_adjuster;
	string_type str{
		buf.data(),
		(len < max_width_acount_sign ? len : max_width_acount_sign)
	};
	if (str.back() == '.')
		str.pop_back();
	return str;
}

Manager::string_type Manager::format_aggregate_() const noexcept
{
	if (!aggregate_)
		return format_error_();

	value_type value = *aggregate_;
	if (boost::abs(value) > max_th)
		return "########";

	return format_value_(value);
}