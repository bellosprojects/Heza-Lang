#include <heza/sets/union.hpp>

namespace heza::sets {

    std::string SetUnion::to_str() const {
        return left_->to_str() + " U " + right_->to_str();
    };

    std::u32string SetUnion::to_latex() const {
        return left_->to_latex() + U" U " + right_->to_latex();
    };

    bool SetUnion::is_equal(const heza::core::ExprPtr& other) const {
        auto self_simplified = expr_to_set(simplify());

        return false;
    };

    heza::core::ExprPtr SetUnion::clone() const {
        return nullptr;
    };

    heza::core::ExprPtr SetUnion::simplify() const {
        return nullptr;
    };

    void SetUnion::accept(const heza::core::VisitorPtr& visitor) const {
        return;
    };

    size_t SetUnion::hash() const {
        return std::size_t();
    };


    bool SetUnion::contains(heza::core::ExprPtr value) const {
        return false;
    };

    bool SetUnion::is_subset(const SetExprPtr& other) const {
        return false;
    };

    bool SetUnion::is_superset(const SetExprPtr& other) const {
        return false;
    };

    bool SetUnion::is_empty() const {
        return false;
    };

    bool SetUnion::is_infinite() const {
        return false;
    };

    bool SetUnion::is_disjoint(const SetExprPtr& other) const {
        return false;
    };

    heza::core::ArithmeticExprPtr SetUnion::cardinality() const {
        return nullptr;
    };

    SetExprPtr SetUnion::complement(const SetExprPtr& universal) const {
        return nullptr;
    };

    SetExprPtr SetUnion::power_set() const {
        return nullptr;
    };

    SetExprPtr SetUnion::symetric_difference(const SetExprPtr& other) const {
        return nullptr;
    };

    SetExprPtr SetUnion::cartesian_product(const SetExprPtr& other) const {
        return nullptr;
    };

    heza::core::ExprPtr SetUnion::choose() const {
        return nullptr;
    };

    int SetUnion::compare(const heza::core::Expr& other) const {
        
        return 1;

    }

    heza::core::TypeId SetUnion::get_type_id() const {
        return heza::core::TypeId::SETUNION;
    }

    heza::core::ExprPtr SetUnion::subs(const heza::core::Substitution& subst) const {

        return std::make_shared<SetUnion>(expr_to_set(left_->subs(subst)), expr_to_set(right_->subs(subst)));
    }

    bool SetUnion::is_atomic() const {
        return false;
    }

};