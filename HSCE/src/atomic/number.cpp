#include <heza/atomic/number.hpp>
#include <heza/algebra/add.hpp>
#include <heza/algebra/mul.hpp>
#include <heza/algebra/pow.hpp>

#include <cmath>

namespace heza::atomic {

    std::string NumberExpr::to_str() const {
        if (std::floor(value_) == value_)
            return std::to_string(static_cast<long long>(value_));
        return std::to_string(value_);
    }

    std::u32string NumberExpr::to_latex() const {
        std::string val = to_str();
        return std::u32string(val.begin(), val.end());
    }

    bool NumberExpr::is_equal(const heza::core::ExprPtr& other) const {
        auto other_num = std::dynamic_pointer_cast<const NumberExpr>(other);  // ← const
        if (!other_num) return false;
        return value_ == other_num->value_;
    }

    heza::core::ExprPtr NumberExpr::clone() const {
        return std::make_shared<NumberExpr>(value_);
    }

    heza::core::ExprPtr NumberExpr::simplify() const {
        return clone();
    }

    void NumberExpr::accept(const heza::core::VisitorPtr& visitor) const {
        // visitor->visit(*this);
    }

    size_t NumberExpr::hash() const {
        size_t h = std::hash<int>{}(static_cast<int>(get_type_id()));
        h ^= std::hash<double>{}(value_) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }

    int NumberExpr::compare(const heza::core::Expr& other) const {
        if (get_type_id() != other.get_type_id()) {
            return static_cast<int>(get_type_id()) < static_cast<int>(other.get_type_id()) ? -1 : 1;
        }
        auto other_num = static_cast<const NumberExpr*>(&other);
        if (value_ < other_num->value_) return -1;
        if (value_ > other_num->value_) return  1;
        return 0;
    }

    heza::core::TypeId NumberExpr::get_type_id() const {
        return heza::core::TypeId::NUMBER;
    }

    heza::core::ExprPtr NumberExpr::subs(const heza::core::Substitution&) const {
        return clone();
    }

    bool NumberExpr::is_atomic() const { return true; }

    heza::core::ArithmeticExprPtr NumberExpr::add(
        const heza::core::ArithmeticExprPtr& other) const
    {
        if (other->is_numeric())
            return make_number(value_ + other->to_double());

        std::vector<heza::core::ArithmeticExprPtr> terms;
        terms.push_back(shared_arith());   // ← sin const_pointer_cast
        terms.push_back(other);
        return std::make_shared<heza::algebra::AddExpr>(std::move(terms));
    }

    heza::core::ArithmeticExprPtr NumberExpr::mul(
        const heza::core::ArithmeticExprPtr& other) const
    {
        if (other->is_numeric())
            return make_number(value_ * other->to_double());

        std::vector<heza::core::ArithmeticExprPtr> factors;
        factors.push_back(shared_arith());   // ← sin const_pointer_cast
        factors.push_back(other);
        return std::make_shared<heza::algebra::MulExpr>(std::move(factors));
    }

    heza::core::ArithmeticExprPtr NumberExpr::pow(
        const heza::core::ArithmeticExprPtr& other) const
    {
        if (other->is_numeric())
            return make_number(std::pow(value_, other->to_double()));
        return std::make_shared<heza::algebra::PowExpr>(shared_arith(), other);
    }

    heza::core::ArithmeticExprPtr NumberExpr::neg() const {
        return make_number(-value_);
    }

    heza::core::ArithmeticExprPtr NumberExpr::diff(
        const heza::atomic::VariableExprPtr&) const
    {
        return make_number(0);
    }

    bool NumberExpr::is_zero()     const { return value_ == 0.0; }
    bool NumberExpr::is_one()      const { return value_ == 1.0; }
    bool NumberExpr::is_negative() const { return value_ < 0; }
    bool NumberExpr::is_positive() const { return value_ > 0; }
    bool NumberExpr::is_numeric()  const { return true; }

    double NumberExpr::to_double() const { return value_; }

    heza::core::ArithmeticExprPtr make_number(double v) {
        return std::make_shared<NumberExpr>(v);
    }

} // namespace heza::atomic