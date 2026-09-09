#pragma once

#define _USE_MATH_DEFINES
#include <pybind11/pybind11.h>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <variant>
#include <complex>
#include <heza/core/expr.hpp>

namespace heza::core {

    class ArithmeticExpr;
    using ArithmeticExprPtr = std::shared_ptr<ArithmeticExpr>;

    ArithmeticExprPtr expr_to_arithmetic(const heza::core::ExprPtr& expr);

    // ---- Clase base ----
    class ArithmeticExpr : public heza::core::Expr {

        public:
            virtual ~ArithmeticExpr() = default;
        
    };

    ArithmeticExprPtr make_number(double v);

};