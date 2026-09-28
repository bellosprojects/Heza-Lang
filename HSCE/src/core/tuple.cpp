#include <heza/core/tuple.hpp>
#include <heza/atomic/number.hpp>

namespace heza::core {
    
    std::string TupleExpr::to_str() const {
        std::string result = "(";
        for(auto i : items_) {
            result.append(i->to_str() + ", ");
        }
        result.pop_back();
        result.pop_back();
        return result + ")";
    };

    std::u32string TupleExpr::to_latex() const {
        std::u32string result = U"(";
        for(auto i : items_) {
            result.append(i->to_latex() + U", ");
        }
        result.pop_back();
        result.pop_back();
        return result + U")";
    };

    bool TupleExpr::is_equal(const ExprPtr& other) const {
        auto other_tuple = std::dynamic_pointer_cast<const TupleExpr>(other);
        if(!other_tuple){
            throw std::runtime_error("Cannot converted");
        }

        if(!cardinality()->is_equal(other_tuple->cardinality())) return false;

        for(int i = 0; i < items_.size(); i ++){
            if(!items_[i]->is_equal(other_tuple->extract(i))) return false;
        }
        return true;
    };

    ExprPtr TupleExpr::clone() const {
        auto new_tuple = std::vector<heza::core::ExprPtr>();
        for(auto i : items_) {
            new_tuple.push_back(i->clone());
        }
        return std::make_shared<TupleExpr>(new_tuple);
    };

    ExprPtr TupleExpr::simplify() const {
        auto new_simplified = std::vector<heza::core::ExprPtr>();
        for(auto i : items_) {
            new_simplified.push_back(i->simplify());
        }
        return std::make_shared<TupleExpr>(new_simplified);
    };

    void TupleExpr::accept(const VisitorPtr& visitor) const {

    };

    size_t TupleExpr::hash() const {
        return std::size_t(items_.size());
    };

    heza::core::ArithmeticExprPtr TupleExpr::cardinality() const {
        return heza::atomic::make_number(items_.size());
    };

    heza::core::ExprPtr TupleExpr::extract(int index) const {
        if(index < 0 || index >= items_.size()){
            throw std::runtime_error("Index out of bounds");
        }
        return items_[index];
    };

    int TupleExpr::compare(const heza::core::Expr& other) const {
        if(get_type_id() != other.get_type_id()) {
            return static_cast<int> (get_type_id()) < static_cast<int>(other.get_type_id()) ? -1 : 1;
        }

        auto other_tuple = static_cast<const TupleExpr *>(&other);

        return -1;
    };

    heza::core::TypeId TupleExpr::get_type_id() const {
        return heza::core::TypeId::TUPLE;
    };

    heza::core::ExprPtr TupleExpr::subs(const heza::core::Substitution& subst) const {
        for(auto t : items_) t->subs(subst);
    };

    bool TupleExpr::is_atomic() const {
        return false;
    };

}