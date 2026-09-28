#pragma once

#include <heza/core/arithmetic.hpp>
namespace heza::algebra {

    class PowExpr;
    using PowExprPtr = std::shared_ptr<const PowExpr>;

    class PowExpr : public heza::core::ArithmeticExpr {

        private:
            heza::core::ArithmeticExprPtr base_;
            heza::core::ArithmeticExprPtr exp_;
            
            heza::core::ArithmeticExprPtr simplify_impl() const;

        public:
            PowExpr(
                const heza::core::ArithmeticExprPtr& base,
                const heza::core::ArithmeticExprPtr& exp
            ): base_(base), exp_(exp) {}

            const heza::core::ArithmeticExprPtr& get_base() const { return base_; }
            const heza::core::ArithmeticExprPtr& get_exp()  const { return exp_; }

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

    };

};