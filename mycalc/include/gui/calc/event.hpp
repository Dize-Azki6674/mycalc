#pragma once

#include <calc/formula/subformula.hpp>

#include <variant>

// guiからは以下の型が入力されることが期待される
struct Number { int value; };	// value: [0, 9]
struct Sign {};
struct Point {};

using Binary = Calc::BinOpr;
struct Equal {};

struct AC {};
struct C {};

// 以下のようなvariantでmanagerに渡される
using CalcEvent = std::variant<
	Number, Sign, Point,
	Binary, Equal,
	AC, C
>;