#pragma once

#include <heza/core/arithmetic.hpp>

namespace heza::atomic {

    class VariableExpr;
    using VariableExprPtr = std::shared_ptr<const VariableExpr>;   // ← const

    class VariableExpr : public heza::core::ArithmeticExpr {
    private:
        std::u32string name_;

    public:
        explicit VariableExpr(std::u32string name) : name_(std::move(name)) {}

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

        const std::u32string& get_name() const { return name_; }
    };

} // namespace heza::atomic