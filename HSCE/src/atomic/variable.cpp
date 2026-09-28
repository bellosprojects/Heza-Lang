#include <heza/atomic/variable.hpp>
#include <heza/algebra/add.hpp>
#include <heza/algebra/mul.hpp>
#include <heza/algebra/pow.hpp>
#include <heza/atomic/number.hpp>
#include <heza/utils/utf.hpp>
#include <stdexcept>

namespace heza::atomic {

    std::string VariableExpr::to_str() const {
        return heza::utils::u32_to_utf8(name_);
    }

    std::u32string VariableExpr::to_latex() const {
        return name_;
    }

    bool VariableExpr::is_equal(const heza::core::ExprPtr& other) const {
        auto other_var = std::dynamic_pointer_cast<const VariableExpr>(other);  // ← const
        return other_var && name_ == other_var->name_;
    }

    heza::core::ExprPtr VariableExpr::clone() const {
        return std::make_shared<VariableExpr>(name_);
    }

    heza::core::ExprPtr VariableExpr::simplify() const {
        return clone();
    }

    void VariableExpr::accept(const heza::core::VisitorPtr&) const {}

    size_t VariableExpr::hash() const {
        size_t h = std::hash<int>{}(static_cast<int>(get_type_id()));
        h ^= std::hash<std::u32string>{}(name_) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }

    int VariableExpr::compare(const heza::core::Expr& other) const {
        if (get_type_id() != other.get_type_id()) {
            return static_cast<int>(get_type_id()) < static_cast<int>(other.get_type_id()) ? -1 : 1;
        }
        auto other_var = static_cast<const VariableExpr*>(&other);
        if (name_ < other_var->name_) return -1;
        if (name_ > other_var->name_) return  1;
        return 0;
    }

    heza::core::TypeId VariableExpr::get_type_id() const {
        return heza::core::TypeId::VARIABLE;
    }

    heza::core::ExprPtr VariableExpr::subs(const heza::core::Substitution& subst) const {
        auto it = subst.find(heza::utils::u32_to_utf8(name_));
        if (it != subst.end()) return it->second;
        return clone();
    }

    bool VariableExpr::is_atomic() const { return true; }

    heza::core::ArithmeticExprPtr VariableExpr::add(
        const heza::core::ArithmeticExprPtr& other) const
    {
        std::vector<heza::core::ArithmeticExprPtr> terms;
        terms.push_back(shared_arith());   // ← sin const_pointer_cast
        terms.push_back(other);
        return std::make_shared<heza::algebra::AddExpr>(std::move(terms));
    }

    heza::core::ArithmeticExprPtr VariableExpr::mul(
        const heza::core::ArithmeticExprPtr& other) const
    {
        std::vector<heza::core::ArithmeticExprPtr> factors;
        factors.push_back(shared_arith());   // ← sin const_pointer_cast
        factors.push_back(other);
        return std::make_shared<heza::algebra::MulExpr>(std::move(factors));
    }

    heza::core::ArithmeticExprPtr VariableExpr::pow(
        const heza::core::ArithmeticExprPtr& other) const
    {
        return std::make_shared<heza::algebra::PowExpr>(shared_arith(), other);
    }

    heza::core::ArithmeticExprPtr VariableExpr::neg() const {
        std::vector<heza::core::ArithmeticExprPtr> factors;
        factors.push_back(make_number(-1));
        factors.push_back(shared_arith());
        return std::make_shared<heza::algebra::MulExpr>(std::move(factors));
    }

    heza::core::ArithmeticExprPtr VariableExpr::diff(
        const heza::atomic::VariableExprPtr& var) const
    {
        if (name_ == var->name_) return make_number(1);
        return make_number(0);
    }

    bool VariableExpr::is_zero()     const { return false; }
    bool VariableExpr::is_one()      const { return false; }
    bool VariableExpr::is_negative() const { return false; }
    bool VariableExpr::is_positive() const { return false; }
    bool VariableExpr::is_numeric()  const { return false; }

    double VariableExpr::to_double() const {
        throw std::runtime_error("No se puede convertir una variable a double");
    }

} // namespace heza::atomic