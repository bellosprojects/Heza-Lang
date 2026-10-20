#pragma once

#include <heza/logic/logic.hpp>

namespace heza::logic {

    class ABoolExpr;
    using ABoolExprPtr = std::shared_ptr<const ABoolExpr>;

    class ABoolExpr : public heza::logic::LogicExpr {

        
        std::string name_;
        public:

        explicit ABoolExpr(std::string name): name_(std::move(name)) {}

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