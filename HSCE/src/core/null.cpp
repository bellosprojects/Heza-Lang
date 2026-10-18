#include <heza/core/nulll.hpp>

namespace heza::core {

    std::string    NullExpr::to_str()   const {
        return "null";
    }

    std::u32string NullExpr::to_latex() const {
        return U"null";
    }

    bool NullExpr::is_equal(const ExprPtr& other) const {
        return std::dynamic_pointer_cast<const NullExpr>(other)? true : false;
    }

    ExprPtr NullExpr::clone()    const {
        return std::make_shared<NullExpr>();
    }

    ExprPtr NullExpr::simplify() const {
        return clone();
    }

    void NullExpr::accept(const VisitorPtr& visitor) const {
        
    }

    size_t NullExpr::hash() const {
        return std::size_t(0l);
    }


    int NullExpr::compare(const Expr& other) const {
        return -1;
    }

    TypeId NullExpr::get_type_id() const {
        return heza::core::TypeId::NULLEXPR;
    }

    ExprPtr NullExpr::subs(const Substitution& subst) const {
        return clone();
    }

    bool NullExpr::is_atomic() const {
        return true;
    }


};