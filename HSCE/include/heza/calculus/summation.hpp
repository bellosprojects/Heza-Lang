#pragma once

#include <heza/core/expr.hpp>
#include <heza/sets/set.hpp>

namespace heza::calculus {

    class SummationExpr;
    using SummationExprPtr = std::shared_ptr<const SummationExpr>;

    class SummationExpr : public heza::core::Expr {

        private:
            heza::sets::SetExprPtr domain_;
            heza::core::ExprPtr expr_;
            std::string var_name_;

        public:
            SummationExpr(const heza::sets::SetExprPtr& domain, const heza::core::ExprPtr& expr, std::string var_name):
            domain_(domain), expr_(expr), var_name_(std::move(var_name)) {}

            std::string    to_str()   const override;
            std::u32string to_latex() const override;
            bool is_equal(const heza::core::ExprPtr& other) const override;
            heza::core::ExprPtr clone()    const override;
            heza::core::ExprPtr simplify() const override;
            void accept(const heza::core::VisitorPtr& visitor) const override;
            size_t hash() const override;

            int compare(const Expr& other) const override;
            heza::core::TypeId get_type_id() const override;
            heza::core::ExprPtr subs(const heza::core::Substitution& subst) const override;
            bool is_atomic() const override;

    };

};