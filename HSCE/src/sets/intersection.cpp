#include <heza/sets/intersection.hpp>

namespace heza::sets {

    std::string SetIntersection::to_str() const {
        return left_->to_str() + " U " + right_->to_str();
    };

    std::u32string SetIntersection::to_latex() const {
        return left_->to_latex() + U" U " + right_->to_latex();
    };

    bool SetIntersection::is_equal(const heza::core::ExprPtr& other) const {
        return false;
    };

    heza::core::ExprPtr SetIntersection::clone() const {
        return nullptr;
    };

    heza::core::ExprPtr SetIntersection::simplify() const {
        return nullptr;
    };

    void SetIntersection::accept(const heza::core::VisitorPtr& visitor) const {
        return;
    };

    size_t SetIntersection::hash() const {
        return std::size_t();
    };


    bool SetIntersection::contains(heza::core::ExprPtr value) const {
        return false;
    };

    bool SetIntersection::is_subset(const SetExprPtr& other) const {
        return false;
    };

    bool SetIntersection::is_superset(const SetExprPtr& other) const {
        return false;
    };

    bool SetIntersection::is_empty() const {
        return false;
    };

    bool SetIntersection::is_infinite() const {
        return false;
    };

    bool SetIntersection::is_disjoint(const SetExprPtr& other) const {
        return false;
    };

    heza::core::ArithmeticExprPtr SetIntersection::cardinality() const {
        return nullptr;
    };

    SetExprPtr SetIntersection::complement(const SetExprPtr& universal) const {
        return nullptr;
    };

    SetExprPtr SetIntersection::power_set() const {
        return nullptr;
    };

    SetExprPtr SetIntersection::symetric_difference(const SetExprPtr& other) const {
        return nullptr;
    };

    SetExprPtr SetIntersection::cartesian_product(const SetExprPtr& other) const {
        return nullptr;
    };

    heza::core::ExprPtr SetIntersection::choose() const {
        return nullptr;
    };

};