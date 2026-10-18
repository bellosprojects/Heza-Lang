#pragma once

#include <heza/core/expr.hpp>

namespace heza::logic {

    class LogicExpr;

    using LogicExprPtr = std::shared_ptr<const LogicExpr>;

    class LogicExpr : public heza::core::Expr {

        public:

        virtual ~LogicExpr() = default;

        LogicExprPtr and_(const LogicExprPtr& other) const;
        LogicExprPtr or_(const LogicExprPtr& other) const; 
        LogicExprPtr not_() const;

    };

};