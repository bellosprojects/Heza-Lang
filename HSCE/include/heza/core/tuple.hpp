#pragma once
#include <heza/core/core.hpp>
#include <memory>
#include <vector>

namespace heza::core {

    class TupleExpr;
    using TupleExprPtr = std::shared_ptr<TupleExpr>;

    class TupleExpr : public heza::core::Expr {

        private:
            std::vector<heza::core::ExprPtr> items_;

        public:
            TupleExpr(std::vector<heza::core::ExprPtr>& items):
                items_(items) {};

            std::string to_str() const;
            std::u32string to_latex() const;
            bool is_equal(const ExprPtr& other) const;
            ExprPtr clone() const;
            ExprPtr simplify() const;
            void accept(const VisitorPtr& visitor) const;
            size_t hash() const;

            heza::core::ArithmeticExprPtr cardinality() const;
            heza::core::ExprPtr extract(int index);
    };

}