#include <heza/sets/difference.hpp>

namespace heza::sets {

    std::string SetDifference::to_str() const {
        return left_->to_str() + " - " + right_->to_str();
    };

    std::u32string SetDifference::to_latex() const {
        return left_->to_latex() + U" - " + right_->to_latex();
    };

    bool SetDifference::is_equal(const heza::core::ExprPtr& other) const {
        return false;
    };

    heza::core::ExprPtr SetDifference::clone() const {
        return nullptr;
    };

    heza::core::ExprPtr SetDifference::simplify() const {
        return nullptr;
    };

    void SetDifference::accept(const heza::core::VisitorPtr& visitor) const {
        return;
    };

    size_t SetDifference::hash() const {
        return std::size_t();
    };


    bool SetDifference::contains(heza::core::ExprPtr value) const {
        return false;
    };

    bool SetDifference::is_subset(const SetExprPtr& other) const {
        return false;
    };

    bool SetDifference::is_superset(const SetExprPtr& other) const {
        return false;
    };

    bool SetDifference::is_empty() const {
        return false;
    };

    bool SetDifference::is_infinite() const {
        return false;
    };

    bool SetDifference::is_disjoint(const SetExprPtr& other) const {
        return false;
    };

    heza::core::ArithmeticExprPtr SetDifference::cardinality() const {
        return nullptr;
    };

    SetExprPtr SetDifference::complement(const SetExprPtr& universal) const {
        return nullptr;
    };

    SetExprPtr SetDifference::power_set() const {
        return nullptr;
    };

    SetExprPtr SetDifference::symetric_difference(const SetExprPtr& other) const {
        return nullptr;
    };

    SetExprPtr SetDifference::cartesian_product(const SetExprPtr& other) const {
        return nullptr;
    };

    heza::core::ExprPtr SetDifference::choose() const {
        return nullptr;
    };

    int SetDifference::compare(const heza::core::Expr& other) const {
        
        return 1;

    }

    heza::core::TypeId SetDifference::get_type_id() const {
        return heza::core::TypeId::SETDIFFERENCE;
    }

    heza::core::ExprPtr SetDifference::subs(const heza::core::Substitution& subst) const {

        return std::make_shared<SetDifference>(expr_to_set(left_->subs(subst)), expr_to_set(right_->subs(subst)));
    }

    bool SetDifference::is_atomic() const {
        return false;
    }

};