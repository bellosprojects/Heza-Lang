#pragma once

#include <heza/logic/logic.hpp>

namespace heza::logic {

    class NotExpr;
    using NotExprPtr = std::shared_ptr<const NotExpr>;

    class NotExpr : public heza::logic::LogicExpr {

        heza::logic::LogicExprPtr expr_;

        public:

        NotExpr(const heza::logic::LogicExprPtr& expr) : expr_(expr) {}

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

    };

};