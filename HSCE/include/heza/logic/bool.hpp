#pragma once

#include <heza/logic/logic.hpp>

namespace heza::logic {

    class BoolExpr;
    using BoolExprPtr = std::shared_ptr<const BoolExpr>;

    class BoolExpr : public heza::logic::LogicExpr {

        
        bool value_;
        public:

        explicit BoolExpr(bool value) : value_(value) {}

        std::string    to_str()   const;
        std::u32string to_latex() const;
        bool is_equal(const heza::core::ExprPtr& other) const;
        heza::core::ExprPtr clone()    const;
        heza::core::ExprPtr simplify() const;
        void accept(const heza::core::VisitorPtr& visitor) const;
        size_t hash() const;

        int compare(const Expr& other) const;
        heza::core::TypeId get_type_id() const;
        heza::core::ExprPtr subs(const heza::core::Substitution& subst) const;
        bool is_atomic() const;
        
        int precedence() const { return 0; }

        bool get_value() const;

    };

};