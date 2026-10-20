#include <heza/logic/abool.hpp>
#include <heza/utils/utf.hpp>

namespace heza::logic {

    std::string    heza::logic::ABoolExpr::to_str()   const {
        return name_;
    }

    std::u32string heza::logic::ABoolExpr::to_latex() const {
        return heza::utils::utf8_to_u32(name_);
    }

    bool heza::logic::ABoolExpr::is_equal(const heza::core::ExprPtr& other) const {
        if(auto other_bool = std::dynamic_pointer_cast<const ABoolExpr>(other)){
            return other_bool->name_ == name_;
        }
        return false;
    }

    heza::core::ExprPtr heza::logic::ABoolExpr::clone()    const {
        return std::make_shared<ABoolExpr>(name_);
    }

    heza::core::ExprPtr heza::logic::ABoolExpr::simplify() const {
        return heza::logic::Simplifier::simplify(this);
    }

    void heza::logic::ABoolExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t heza::logic::ABoolExpr::hash() const {
        return std::size_t(0l);
    }


    int heza::logic::ABoolExpr::compare(const Expr& other) const {
        if(get_type_id() != other.get_type_id()) return (int)get_type_id() - (int)other.get_type_id();
        auto& b = static_cast<const ABoolExpr&>(other);
        int r = name_.compare(b.name_);
        return r < 0 ? -1 : (r > 0 ? 1 : 0);
    }

    heza::core::TypeId heza::logic::ABoolExpr::get_type_id() const {
        return heza::core::TypeId::ABOOL;
    }

    heza::core::ExprPtr heza::logic::ABoolExpr::subs(const heza::core::Substitution& subst) const {
        auto it = subst.find(name_);

        if(it != subst.end()) return it->second;
        return clone();
    }

    bool heza::logic::ABoolExpr::is_atomic() const {
        return true;
    }

};