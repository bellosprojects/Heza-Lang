#pragma once

#include <memory>
#include <string>
#include <functional>
#include <map>
#include <heza/core/visitor.hpp>

namespace heza::core {

    class Expr;
    using ExprPtr = std::shared_ptr<Expr>;
    using Substitution = std::map<std::string, ExprPtr>;

    class Expr : public std::enable_shared_from_this<Expr> {

        public:
            virtual ~Expr() = default;

            virtual std::string to_str() const = 0;
            virtual std::u32string to_latex() const = 0;
            virtual bool is_equal(const ExprPtr& other) const = 0;
            virtual ExprPtr clone() const = 0;
            virtual ExprPtr simplify() const = 0;
            virtual void accept(const VisitorPtr& visitor) const = 0;
            virtual size_t hash() const = 0;
    };

};