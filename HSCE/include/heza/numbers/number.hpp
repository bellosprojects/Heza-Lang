#pragma once

#include <heza/core/core.hpp>

namespace heza::numbers {

    class NumberExpr;
    using NumberExprPtr = std::shared_ptr<NumberExpr>;

    class NumberExpr : public heza::core::ArithmeticExpr {

        private:
            double value_;

        public:
            NumberExpr(const double& value) : value_(value) {};

            std::string to_str() const;
            std::u32string to_latex() const;
            bool is_equal(const heza::core::ExprPtr& other) const;
            heza::core::ExprPtr clone() const;
            heza::core::ExprPtr simplify() const;
            void accept(const heza::core::VisitorPtr& visitor) const;
            size_t hash() const;

            double get_value() const;
    };

}