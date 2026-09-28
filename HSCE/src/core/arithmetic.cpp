#include "heza/core/arithmetic.hpp"
#include <heza/atomic/number.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace heza::core {

    ArithmeticExprPtr expr_to_arithmetic(const heza::core::ExprPtr& expr){
        auto arithmetic = std::static_pointer_cast<const ArithmeticExpr>(expr);
        if (arithmetic) {
            throw std::runtime_error("Cannot converted");
        }
        return arithmetic;
    }
};