#pragma once

#include <memory>
#include <string>
#include <functional>
#include <map>
#include <cstdint>
#include <heza/core/visitor.hpp>
#include <pybind11/pybind11.h>

namespace heza::core {

    class Expr;
    using ExprPtr = std::shared_ptr<const Expr>;
    using Substitution = std::map<std::string, ExprPtr>;

    enum class TypeId : uint16_t {
        // HOJAS
        NUMBER, VARIABLE, CONSTANT, INFINITE, INDETERMINATE, BOOL, TEXT,
        // AGRUPADORES
        ADD, MUL,
        // BINARIOS
        POW, LOG,
        // UNARIOS
        EXP, SIN, COS, TAN, COT, SEC, CSC,
        ARCSIN, ARCCOS, ARCTAN, ARCCOT, ARCSEC, ARCCSC,
        SINH, COSH, TANH, COTH, SECH, CSCH,
        ARCSINH, ARCCOSH, ARCTANH, ARCCOTH, ARCSECH, ARCCSCH,
        // TUPLAS
        TUPLE,
        // CONJUNTOS
        FINITESET, EMPTYSET, SETUNION, SETINTERSECTION, SETDIFFERENCE,
        SETREALS, SETNATURALS, SETINTEGERS, SETRATIONALS,
        // ITERADORES
        SUMMATION, PRODUCTION,
        // CALCULO
        DERIVATIVE, INTEGRATE, LIMIT,
        // LOGICA
        AND, OR, NOT, THEN, FORALL, EXISTS,
        // RELATIONALS
        IN, NOTIN, SUBSET, EQSUBSET, EQUAL, NOTEQUAL,
        GREATER, EQGREATER, LESS, EQLESS
    };

    class Expr : public std::enable_shared_from_this<Expr> {
    public:
        virtual ~Expr() = default;

        virtual std::string    to_str()   const = 0;
        virtual std::u32string to_latex() const = 0;
        virtual bool is_equal(const ExprPtr& other) const = 0;
        virtual ExprPtr clone()    const = 0;
        virtual ExprPtr simplify() const = 0;
        virtual void accept(const VisitorPtr& visitor) const = 0;
        virtual size_t hash() const = 0;

        virtual int compare(const Expr& other) const = 0;
        virtual TypeId get_type_id() const = 0;
        virtual ExprPtr subs(const Substitution& subst) const = 0;
        virtual bool is_atomic() const = 0;
    };

} // namespace heza::core