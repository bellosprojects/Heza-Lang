#include <heza/sets/abstractset.hpp>
#include <heza/core/expr.hpp>
#include <heza/utils/utf.hpp>
#include <heza/atomic/variable.hpp>
#include <heza/sets/difference.hpp>

namespace heza::sets {

    std::string AbstractSetExpr::to_str() const {
        return name_;
    }

    std::u32string AbstractSetExpr::to_latex() const {
        return heza::utils::utf8_to_u32(name_);
    }

    bool AbstractSetExpr::is_equal(const heza::core::ExprPtr& other) const {
        if(auto other_as = std::dynamic_pointer_cast<const AbstractSetExpr>(expr_to_set(other))){
            return other_as->name_ == name_;
        }
        return false;
    }

    heza::core::ExprPtr AbstractSetExpr::clone() const {
        return std::make_shared<AbstractSetExpr>(std::move(name_));
    }

    heza::core::ExprPtr AbstractSetExpr::simplify() const {
        return std::make_shared<AbstractSetExpr>(std::move(name_));
    }

    void AbstractSetExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t AbstractSetExpr::hash() const {
        return std::size_t(0l);
    }


    bool AbstractSetExpr::contains(heza::core::ExprPtr value) const {
        return false;
    }

    bool AbstractSetExpr::is_subset(const SetExprPtr& other) const {
        return false;
    }

    bool AbstractSetExpr::is_superset(const SetExprPtr& other) const {
        return false;
    }

    bool AbstractSetExpr::is_empty() const {
        return false;
    }

    bool AbstractSetExpr::is_infinite() const {
        return false;
    }

    bool AbstractSetExpr::is_disjoint(const SetExprPtr& other) const {
        return false;
    }

    heza::core::ArithmeticExprPtr AbstractSetExpr::cardinality() const {
        return std::make_shared<heza::atomic::VariableExpr>(U"size_of_" + heza::utils::utf8_to_u32(name_));
    }

    SetExprPtr AbstractSetExpr::complement(const SetExprPtr& universal) const {
        return std::make_shared<SetDifference>(universal, std::static_pointer_cast<const SetExpr>(shared_from_this()));
    }

    SetExprPtr AbstractSetExpr::power_set() const {
        return std::make_shared<AbstractSetExpr>(std::move(name_));
    }

    SetExprPtr AbstractSetExpr::symetric_difference(const SetExprPtr& other) const {
        return std::make_shared<AbstractSetExpr>(std::move(name_));
    }

    SetExprPtr AbstractSetExpr::cartesian_product(const SetExprPtr& other) const {
        return std::make_shared<AbstractSetExpr>(std::move(name_));
    }

    heza::core::ExprPtr AbstractSetExpr::choose() const {
        return clone();
    }


    int AbstractSetExpr::compare(const heza::core::Expr& other) const {
        return -1;
    }

    heza::core::TypeId AbstractSetExpr::get_type_id() const {
        return heza::core::TypeId::ABSTRACTSET;
    }

    heza::core::ExprPtr AbstractSetExpr::subs(const heza::core::Substitution& subst) const {
        return clone();
    }

    bool AbstractSetExpr::is_atomic() const {
        return true;
    }

};