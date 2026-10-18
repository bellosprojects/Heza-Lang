#include <heza/logic/nott.hpp>
#include <heza/logic/bool.hpp>

namespace heza::logic {

    std::string    NotExpr::to_str()   const {

        return "\\not " + expr_->to_str();
    }

    std::u32string NotExpr::to_latex() const {
        return U"not(" + expr_->to_latex() + U")";
    }

    bool NotExpr::is_equal(const heza::core::ExprPtr& other) const {
        if(auto other_not = std::dynamic_pointer_cast<const NotExpr>(other)){
            return expr_->is_equal(other_not->expr_);
        }
        return false;
    }

    heza::core::ExprPtr NotExpr::clone()    const {
        return std::make_shared<NotExpr>(expr_);
    }

    heza::core::ExprPtr NotExpr::simplify() const {

        if(auto inner_bool = std::dynamic_pointer_cast<const heza::logic::BoolExpr>(expr_)){
            return std::make_shared<heza::logic::BoolExpr>(!inner_bool->get_value());
        }

        if(auto inner_not = std::dynamic_pointer_cast<const NotExpr>(expr_)){
            return inner_not->expr_;
        } else {
            return clone();
        }
    }

    void NotExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t NotExpr::hash() const {
        return std::size_t(0L);
    }


    int NotExpr::compare(const Expr& other) const {
        return -1;
    }

    heza::core::TypeId NotExpr::get_type_id() const {
        return heza::core::TypeId::NOT;
    }

    heza::core::ExprPtr NotExpr::subs(const heza::core::Substitution& subst) const {
        if(auto expr_subs = std::dynamic_pointer_cast<const LogicExpr>(expr_->subs(subst))){
            return std::make_shared<NotExpr>(expr_subs);
        } else {
            throw std::runtime_error("NotExpr::subs: termino no logico");
        }
    }

    bool NotExpr::is_atomic() const {
        return false;
    }

};