#include <heza/sets/set.hpp>

namespace heza::sets {
    SetExprPtr expr_to_set(const heza::core::ExprPtr& expr){
        auto val = std::dynamic_pointer_cast<SetExpr>(expr);
        if(!val) throw std::runtime_error("Cannot converted");
        return val;
    }
}