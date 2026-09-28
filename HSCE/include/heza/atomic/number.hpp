#pragma once

#include <heza/core/arithmetic.hpp>

namespace heza::atomic {

    class NumberExpr;
    using NumberExprPtr = std::shared_ptr<const NumberExpr>;   // ← const

    class NumberExpr : public heza::core::ArithmeticExpr {
    private:
        double value_;

    public:
        explicit NumberExpr(double value) : value_(value) {}

        std::string    to_str()    const override;
        std::u32string to_latex()  const override;
        bool is_equal(const heza::core::ExprPtr& other) const override;
        heza::core::ExprPtr clone()    const override;
        heza::core::ExprPtr simplify() const override;
        void accept(const heza::core::VisitorPtr& visitor) const override;
        size_t hash() const override;

        int compare(const heza::core::Expr& other) const override;
        heza::core::TypeId get_type_id() const override;
        heza::core::ExprPtr subs(const heza::core::Substitution& subst) const override;
        bool is_atomic() const override;

        heza::core::ArithmeticExprPtr add(const heza::core::ArithmeticExprPtr& other) const override;
        heza::core::ArithmeticExprPtr mul(const heza::core::ArithmeticExprPtr& other) const override;
        heza::core::ArithmeticExprPtr pow(const heza::core::ArithmeticExprPtr& other) const override;
        heza::core::ArithmeticExprPtr neg() const override;

        heza::core::ArithmeticExprPtr diff(const heza::atomic::VariableExprPtr& var) const override;

        bool is_zero()     const override;
        bool is_one()      const override;
        bool is_negative() const override;
        bool is_positive() const override;
        bool is_numeric()  const override;

        double to_double() const override;

        double get_value() const { return value_; }
    };

    heza::core::ArithmeticExprPtr make_number(double v);

} // namespace heza::atomic