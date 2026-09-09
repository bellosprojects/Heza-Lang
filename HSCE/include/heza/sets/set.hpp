#pragma once

#include <memory>
#include <heza/core/core.hpp>
#include <string>
#include <functional> 

namespace heza::sets {

    class SetExpr;
    using SetExprPtr = std::shared_ptr<SetExpr>;

    SetExprPtr expr_to_set(const heza::core::ExprPtr& expr);

    class SetExpr : public heza::core::Expr {

        public:
            virtual ~SetExpr() = default;

            virtual bool contains(heza::core::ExprPtr value) const = 0;
            virtual bool is_subset(const SetExprPtr& other) const = 0;
            virtual bool is_superset(const SetExprPtr& other) const = 0;
            virtual bool is_empty() const = 0;
            virtual bool is_infinite() const = 0;
            virtual bool is_disjoint(const SetExprPtr& other) const = 0;
            virtual heza::core::ArithmeticExprPtr cardinality() const = 0;
            virtual SetExprPtr complement(const SetExprPtr& universal) const = 0;
            virtual SetExprPtr power_set() const = 0;
            virtual SetExprPtr symetric_difference(const SetExprPtr& other) const = 0;
            virtual SetExprPtr cartesian_product(const SetExprPtr& other) const = 0;
            virtual heza::core::ExprPtr choose() const = 0;

            SetExprPtr set_union(const SetExprPtr& other) const;
            SetExprPtr intersect(const SetExprPtr& other) const;
            SetExprPtr difference(const SetExprPtr& other) const;
    };

};