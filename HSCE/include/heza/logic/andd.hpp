#pragma once

#include <heza/logic/logic.hpp>

namespace heza::logic {

    class AndExpr;
    using AndExprPtr = std::shared_ptr<const AndExpr>;

    class AndExpr : public heza::logic::LogicExpr {

        std::vector<heza::logic::LogicExprPtr> exprs_;

        public:

        AndExpr(std::vector<heza::logic::LogicExprPtr> exprs);

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