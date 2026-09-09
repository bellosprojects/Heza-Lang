#pragma once

#include <heza/core/core.hpp>
#include <heza/sets/set.hpp>
#include <memory>
#include <set>

namespace heza::sets {

    class FiniteSet;
    using FiniteSetPtr = std::shared_ptr<FiniteSet>;

    class FiniteSet : public SetExpr {

        private:
            std::set<heza::core::ExprPtr> items_;

        public:
            FiniteSet(std::set<heza::core::ExprPtr>& items): items_(items) {};

            std::string to_str() const;
            std::u32string to_latex() const;
            bool is_equal(const heza::core::ExprPtr& other) const;
            heza::core::ExprPtr clone() const;
            heza::core::ExprPtr simplify() const;
            void accept(const heza::core::VisitorPtr& visitor) const;
            size_t hash() const;

            bool contains(heza::core::ExprPtr value) const;
            bool is_subset(const SetExprPtr& other) const;
            bool is_superset(const SetExprPtr& other) const;
            bool is_empty() const;
            bool is_infinite() const;
            bool is_disjoint(const SetExprPtr& other) const;
            heza::core::ArithmeticExprPtr cardinality() const;
            SetExprPtr complement(const SetExprPtr& universal) const;
            SetExprPtr power_set() const;
            SetExprPtr symetric_difference(const SetExprPtr& other) const;
            SetExprPtr cartesian_product(const SetExprPtr& other) const;
            heza::core::ExprPtr choose() const;
    };

};