#pragma once

#include <heza/core/expr.hpp>
#include <heza/atomic/number.hpp>

namespace heza::core {

    class TextExpr;
    using TextExprPtr = std::shared_ptr<const TextExpr>;

    class TextExpr : public heza::core::Expr {

        std::u32string value_;

        public:

        TextExpr(std::u32string value) : value_(std::move(value)) {}

        std::string    to_str()   const;
        std::u32string to_latex() const;
        bool is_equal(const ExprPtr& other) const;
        ExprPtr clone()    const;
        ExprPtr simplify() const;
        void accept(const VisitorPtr& visitor) const;
        size_t hash() const;

        int compare(const Expr& other) const;
        TypeId get_type_id() const;
        ExprPtr subs(const Substitution& subst) const;
        bool is_atomic() const;

        TextExprPtr concat(const TextExprPtr& other) const;
        TextExprPtr repeat(const heza::atomic::NumberExprPtr& times) const;
        heza::atomic::NumberExprPtr length() const;
        TextExprPtr charAt(const heza::atomic::NumberExprPtr& index) const;

    };

};