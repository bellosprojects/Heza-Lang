#include <heza/numbers/number.hpp>

#include <cmath>

namespace heza::numbers {

    std::string NumberExpr::to_str() const {
        if (std::floor(value_) == value_) {
            return std::to_string(static_cast<long long>(value_));
        }
        return std::to_string(value_);
    };

    std::u32string NumberExpr::to_latex() const {
        std::string val = to_str();
        return std::u32string(val.begin(), val.end());
    };

    bool NumberExpr::is_equal(const heza::core::ExprPtr& other) const {
        auto other_num = std::dynamic_pointer_cast<NumberExpr>(other);
        if(!other_num) return false;
        return value_ == other_num->get_value();
    };

    heza::core::ExprPtr NumberExpr::clone() const {
        return heza::core::make_number(value_);
    };

    heza::core::ExprPtr NumberExpr::simplify() const {
        return std::const_pointer_cast<heza::core::Expr>(shared_from_this());
    };

    void NumberExpr::accept(const heza::core::VisitorPtr& visitor) const {

    };

    size_t NumberExpr::hash() const {
        return size_t(value_);
    };

    double NumberExpr::get_value() const {
        return value_;
    }
}