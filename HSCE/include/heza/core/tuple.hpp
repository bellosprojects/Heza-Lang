#pragma once
#include <heza/core/core.hpp>
#include <memory>
#include <vector>

namespace heza::core {

    class TupleExpr;
    using TupleExprPtr = std::shared_ptr<const TupleExpr>;

    class TupleExpr : public heza::core::Expr {

        private:
            std::vector<heza::core::ExprPtr> items_;

        public:
            TupleExpr(std::vector<heza::core::ExprPtr>& items):
                items_(items) {};

            std::string to_str() const override;
            std::u32string to_latex() const override;
            bool is_equal(const ExprPtr& other) const override;
            ExprPtr clone() const override;
            ExprPtr simplify() const override;
            void accept(const VisitorPtr& visitor) const override;
            size_t hash() const override;
            int compare(const heza::core::Expr& other) const override;
            heza::core::TypeId get_type_id() const override;
            heza::core::ExprPtr subs(const heza::core::Substitution& subst) const override;
            bool is_atomic() const override;

            heza::core::ArithmeticExprPtr cardinality() const;
            heza::core::ExprPtr extract(int index) const;
    };

}