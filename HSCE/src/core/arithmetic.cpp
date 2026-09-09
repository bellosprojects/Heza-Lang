#include "heza/core/arithmetic.hpp"
#include <heza/numbers/number.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace heza::core {

    ArithmeticExprPtr make_number(double v){
        return std::make_shared<heza::numbers::NumberExpr>(v);
    }

};