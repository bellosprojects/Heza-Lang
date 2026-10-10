#include <heza/calculus/summation.hpp>
#include <heza/utils/utf.hpp>
#include <heza/atomic/number.hpp>
#include <heza/sets/finiteset.hpp>
#include <heza/algebra/add.hpp>

namespace heza::calculus {

    std::string    SummationExpr::to_str()   const {
        return "\\sum_{" + var_name_ + " \\in {" + domain_->to_str() + "}}{" + expr_->to_str() + "}";
    }

    std::u32string SummationExpr::to_latex() const {
        return U"∑(" + heza::utils::utf8_to_u32(var_name_) + U"∈" + domain_->to_latex() + U", " + expr_->to_latex() + U")";
    }

    bool SummationExpr::is_equal(const heza::core::ExprPtr& other) const {

        if(get_type_id() != other->get_type_id()) return false;

        auto other_sum = std::static_pointer_cast<const SummationExpr>(other);

        if(!other_sum) return false;

        return domain_->is_equal(other_sum->domain_) && expr_->is_equal(other_sum->expr_) && var_name_ == other_sum->var_name_;
    }

    heza::core::ExprPtr SummationExpr::clone()    const {
        return std::make_shared<SummationExpr>(
            heza::sets::expr_to_set(domain_->clone()),
            expr_->clone(),
            std::move(var_name_)
        );
    }

    heza::core::ExprPtr SummationExpr::simplify() const {

        auto simplified_domain = domain_->simplify();
        auto simplified_expr = expr_->simplify();

        if (auto finite = std::dynamic_pointer_cast<const heza::sets::FiniteSet>(simplified_domain))
        {
            std::vector<heza::core::ArithmeticExprPtr> result;
    
            for(auto item : finite->items_){

                heza::core::Substitution env;
                env.insert(heza::core::Substitution::value_type(var_name_, item));

                auto inner = std::dynamic_pointer_cast<const heza::core::ArithmeticExpr>(expr_->subs(env));
                if(inner) result.push_back(inner);
                else throw std::runtime_error("Expresion invalida en Summation");
                
            }

            return std::make_shared<heza::algebra::AddExpr>(result)->simplify();
        }
        
        return std::make_shared<SummationExpr>(
            heza::sets::expr_to_set(simplified_domain),
            simplified_expr,
            var_name_
        );
    }

    void SummationExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t SummationExpr::hash() const {
        return std::size_t(0);
    }


    int SummationExpr::compare(const Expr& other) const {
        return -1;
    }

    heza::core::TypeId SummationExpr::get_type_id() const {
        return heza::core::TypeId::SUMMATION;
    }

    heza::core::ExprPtr SummationExpr::subs(const heza::core::Substitution& subst) const {
        return clone();
    }

    bool SummationExpr::is_atomic() const {
        return false;
    }


};