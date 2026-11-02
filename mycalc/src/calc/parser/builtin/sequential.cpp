#include <calc/parser/builtin/sequential.hpp>

#include <memory>
#include <utility>
#include <variant>

namespace Calc {

    namespace {

        std::optional<SubFormula>
            make_subformula(
                SubFormula lhs,
                const Token& op,
                const Token& value)
        {
            return std::visit(
                [&lhs](const auto& op_, const auto& value_)
                -> std::optional<SubFormula>
                {
                    using Op = std::decay_t<decltype(op_)>;
                    using Value = std::decay_t<decltype(value_)>;

                    if constexpr (
                        std::same_as<Op, BinOpr> &&
                        std::same_as<Value, ValueType>)
                    {
                        SubFormula rhs{ value_ };

                        switch (op_) {
                        case BinOpr::Add:
                            return SubFormula{
                                std::make_unique<Add>(
                                    std::move(lhs),
                                    std::move(rhs))
                            };

                        case BinOpr::Sub:
                            return SubFormula{
                                std::make_unique<Sub>(
                                    std::move(lhs),
                                    std::move(rhs))
                            };

                        case BinOpr::Mul:
                            return SubFormula{
                                std::make_unique<Mul>(
                                    std::move(lhs),
                                    std::move(rhs))
                            };

                        case BinOpr::Div:
                            return SubFormula{
                                std::make_unique<Div>(
                                    std::move(lhs),
                                    std::move(rhs))
                            };

                        default:
                            std::unreachable();
                        }
                    }
                    else {
                        return std::nullopt;
                    }
                },
                op,
                value);
        }

    }

    std::optional<SubFormula>
        Sequential::parse(const std::vector<Token>& tokens)
    {
        if (tokens.empty())
            return std::nullopt;

        if (!std::holds_alternative<ValueType>(tokens.front()))
            return std::nullopt;

        SubFormula formula{
            std::get<ValueType>(tokens.front())
        };

        for (std::size_t i = 1; i < tokens.size(); i += 2) {
            if (i + 1 >= tokens.size())
                return std::nullopt;

            auto next = make_subformula(
                std::move(formula),
                tokens[i],
                tokens[i + 1]);

            if (!next)
                return std::nullopt;

            formula = std::move(*next);
        }

        return formula;
    }

}