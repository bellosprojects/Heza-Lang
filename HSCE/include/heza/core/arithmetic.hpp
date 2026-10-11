#pragma once

#include <memory>
#include <vector>
#include <string>
#include <heza/core/expr.hpp>

namespace heza::atomic {
    class VariableExpr;
    using VariableExprPtr = std::shared_ptr<const VariableExpr>;   // ← const
}

namespace heza::core {

    class ArithmeticExpr;
    using ArithmeticExprPtr = std::shared_ptr<const ArithmeticExpr>;  // ← const

    class ArithmeticExpr : public Expr {
    public:
        virtual ~ArithmeticExpr() = default;

        virtual ArithmeticExprPtr add(const ArithmeticExprPtr& other) const = 0;
        virtual ArithmeticExprPtr mul(const ArithmeticExprPtr& other) const = 0;
        virtual ArithmeticExprPtr pow(const ArithmeticExprPtr& other) const = 0;
        virtual ArithmeticExprPtr neg() const = 0;

        virtual ArithmeticExprPtr diff(const heza::atomic::VariableExprPtr& var) const = 0;

        virtual bool is_zero()     const = 0;
        virtual bool is_one()      const = 0;
        virtual bool is_negative() const = 0;
        virtual bool is_positive() const = 0;
        virtual bool is_numeric()  const = 0;

        virtual double to_double() const = 0;

        ArithmeticExprPtr simplify_arith() const {
            return std::static_pointer_cast<const ArithmeticExpr>(this->simplify());
        }
        
    protected:
        ArithmeticExprPtr shared_arith() const {
            return std::static_pointer_cast<const ArithmeticExpr>(
                this->shared_from_this());
        }

    };

} // namespace heza::core