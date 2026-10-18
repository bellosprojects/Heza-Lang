#include <heza/logic/bool.hpp>

namespace heza::logic {

    std::string    heza::logic::BoolExpr::to_str()   const {
        return value_? "true" : "false";
    }

    std::u32string heza::logic::BoolExpr::to_latex() const {
        return value_? U"true" : U"false";
    }

    bool heza::logic::BoolExpr::is_equal(const heza::core::ExprPtr& other) const {
        if(auto other_bool = std::dynamic_pointer_cast<const BoolExpr>(other)){
            return other_bool->value_ == value_;
        }
        return false;
    }

    heza::core::ExprPtr heza::logic::BoolExpr::clone()    const {
        return std::make_shared<BoolExpr>(value_);
    }

    heza::core::ExprPtr heza::logic::BoolExpr::simplify() const {
        return clone();
    }

    void heza::logic::BoolExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t heza::logic::BoolExpr::hash() const {
        return std::size_t(0l);
    }


    int heza::logic::BoolExpr::compare(const Expr& other) const {
        return -1;
    }

    heza::core::TypeId heza::logic::BoolExpr::get_type_id() const {
        return heza::core::TypeId::BOOL;
    }

    heza::core::ExprPtr heza::logic::BoolExpr::subs(const heza::core::Substitution& subst) const {
        return clone();
    }

    bool heza::logic::BoolExpr::is_atomic() const {
        return true;
    }

    bool heza::logic::BoolExpr::get_value() const {
        return value_;
    }

};