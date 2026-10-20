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
        return heza::logic::Simplifier::simplify(this);
    }

    void heza::logic::BoolExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t heza::logic::BoolExpr::hash() const {
        return std::size_t(0l);
    }


    int heza::logic::BoolExpr::compare(const Expr& other) const {
        if(get_type_id() != other.get_type_id()) return (int)get_type_id() - (int)other.get_type_id();
        auto& b = static_cast<const BoolExpr&>(other);
        if(value_ == b.value_) return 0;
        return value_ ? 1 : -1;
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