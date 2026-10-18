#pragma once

#include <heza/core/expr.hpp>

namespace heza::core {

    class NullExpr;
    using NullExprPtr = std::shared_ptr<const NullExpr>;

    class NullExpr : public heza::core::Expr {

        public:

        NullExpr() {}

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

    };

};