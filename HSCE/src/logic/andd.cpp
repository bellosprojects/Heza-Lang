#include <heza/logic/andd.hpp>

namespace heza::logic {

    AndExpr::AndExpr(std::vector<heza::logic::LogicExprPtr> exprs) {

        std::vector<heza::logic::LogicExprPtr> flatten;

        for(auto e : exprs){

            if (auto inner_and = std::dynamic_pointer_cast<const AndExpr>(e))
            {
                flatten.insert(flatten.end(), inner_and->exprs_.begin(), inner_and->exprs_.end());
            }
            else {
                flatten.push_back(e);
            }

        }

        exprs_ = flatten;
    }

    std::string    AndExpr::to_str()   const {

        if (exprs_.empty()) return "true";

        std::string result = "(" + exprs_[0]->to_str();

        for(size_t i = 1; i < exprs_.size(); i ++){
            result += " \\wedge ";
            result += exprs_[i]->to_str();
        }

        return result + ")";
    }

    std::u32string AndExpr::to_latex() const {
        if (exprs_.empty()) return U"true";

        std::u32string result = U"(" + exprs_[0]->to_latex();

        for(size_t i = 1; i < exprs_.size(); i ++){
            result += U" \\vee ";
            result += exprs_[i]->to_latex();
        }

        return result + U")";
    }

    bool AndExpr::is_equal(const heza::core::ExprPtr& other) const {
        return false;
    }

    heza::core::ExprPtr AndExpr::clone()    const {
        return std::make_shared<AndExpr>(exprs_);
    }

    heza::core::ExprPtr AndExpr::simplify() const {
        return clone();
    }

    void AndExpr::accept(const heza::core::VisitorPtr& visitor) const {
        
    }

    size_t AndExpr::hash() const {
        return std::size_t(0L);
    }


    int AndExpr::compare(const Expr& other) const {
        return -1;
    }

    heza::core::TypeId AndExpr::get_type_id() const {
        return heza::core::TypeId::AND;
    }

    heza::core::ExprPtr AndExpr::subs(const heza::core::Substitution& subst) const {
        std::vector<heza::logic::LogicExprPtr> new_exprs;

        for (auto e : exprs_)
        {
            if(auto e_subs = std::dynamic_pointer_cast<const LogicExpr>(e->subs(subst))){
                new_exprs.push_back(e_subs);
            } else {
                throw std::runtime_error("And::subs: termino no logico");
            }
        }
        
        return std::make_shared<AndExpr>(new_exprs);
    }

    bool AndExpr::is_atomic() const {
        return false;
    }


};