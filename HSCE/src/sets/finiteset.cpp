#include <heza/sets/finiteset.hpp>

namespace heza::sets {

    std::string FiniteSet::to_str() const {
        std::string result = "{";
        for(auto i : items_){
            result.append(i->to_str());
            result.append(", ");
        }
        result.pop_back();
        result.pop_back();
        result.append("}");
        return result;
    }

    std::u32string FiniteSet::to_latex() const {
        std::u32string result = U"{";
        for(auto i : items_){
            result.append(i->to_latex());
            result.append(U", ");
        }
        result.pop_back();
        result.pop_back();
        result.append(U"}");
        return result;
    }

    bool FiniteSet::is_equal(const heza::core::ExprPtr& other) const {

        auto other_set = std::dynamic_pointer_cast<FiniteSet>(other);
        if(!other_set) return false;

        if(!cardinality()->is_equal(other_set->cardinality())) return false;

        for(auto i : items_){
            if(!other_set->contains(i)) return false;
        }

        return true;
    };

    heza::core::ExprPtr FiniteSet::clone() const {
        auto new_set = std::set<heza::core::ExprPtr>();
        for(auto i : items_){
            new_set.insert(i->clone());
        }
        return std::make_shared<FiniteSet>(new_set);
    };

    heza::core::ExprPtr FiniteSet::simplify() const {
        auto simplified_set = std::set<heza::core::ExprPtr>();
        for(auto i : items_){
            simplified_set.insert(i->simplify());
        }
        return std::make_shared<FiniteSet>(simplified_set);
    };

    void FiniteSet::accept(const heza::core::VisitorPtr& visitor) const {

    };

    size_t FiniteSet::hash() const {
        return std::size_t(items_.size());
    };

    bool FiniteSet::contains(heza::core::ExprPtr value) const {
        for(auto i : items_){
            if(i->is_equal(value)) return true;
        }
        return false;
    };

    bool FiniteSet::is_subset(const SetExprPtr& other) const {
        for(auto i : items_){
            if(!other->contains(i)) return false;
        }
        return true;
    };

    bool FiniteSet::is_superset(const SetExprPtr& other) const {
        return other->is_subset(std::static_pointer_cast<SetExpr>(
            std::const_pointer_cast<heza::core::Expr>(shared_from_this())));
    };

    bool FiniteSet::is_empty() const {
        return items_.size() == 0;
    };

    bool FiniteSet::is_infinite() const {
        return false;
    };

    bool FiniteSet::is_disjoint(const SetExprPtr& other) const {
        for(auto i : items_){
            if(other->contains(i)) return false;
        }
        return true;
    };

    heza::core::ArithmeticExprPtr FiniteSet::cardinality() const {
        return heza::core::make_number(items_.size());
    };

    SetExprPtr FiniteSet::complement(const SetExprPtr& universal) const {
        return nullptr;
    };

    SetExprPtr FiniteSet::power_set() const {
        return nullptr;
    };

    SetExprPtr FiniteSet::symetric_difference(const SetExprPtr& other) const {
        return nullptr;
    };

    SetExprPtr FiniteSet::cartesian_product(const SetExprPtr& other) const {
        return nullptr;
    };

    heza::core::ExprPtr FiniteSet::choose() const {
        return nullptr;
    };

};