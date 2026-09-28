#include <heza/algebra/pow.hpp>
#include <heza/algebra/add.hpp>
#include <heza/algebra/mul.hpp>
#include <heza/atomic/number.hpp>
#include <heza/atomic/variable.hpp>
#include <heza/utils/utf.hpp>

#include <cmath>
#include <stdexcept>

namespace heza::algebra {

    using core::ArithmeticExprPtr;
    using core::ExprPtr;
    using core::Substitution;
    using core::TypeId;
    using atomic::make_number;
    using atomic::NumberExpr;
    using atomic::VariableExprPtr;

    std::string PowExpr::to_str() const {
        return "(" + base_->to_str() + ")^(" + exp_->to_str() + ")";
    }

    std::u32string PowExpr::to_latex() const {
        std::u32string b = base_->to_latex();
        std::u32string e = exp_->to_latex();
        std::u32string r;
        r.reserve(b.size() + e.size() + 5);
        r += U"{";
        r += b;
        r += U"}^{";
        r += e;
        r += U"}";
        return r;
    }

    bool PowExpr::is_equal(const ExprPtr& other) const {
        auto o = std::dynamic_pointer_cast<const PowExpr>(other);
        if (!o) return false;
        return base_->is_equal(o->base_) && exp_->is_equal(o->exp_);
    }

    ExprPtr PowExpr::clone() const {
        return std::make_shared<PowExpr>(base_, exp_);
    }

    size_t PowExpr::hash() const {
        size_t h = std::hash<int>{}(static_cast<int>(get_type_id()));
        h ^= base_->hash() + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        h ^= exp_->hash()  + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }

    int PowExpr::compare(const core::Expr& other) const {
        if (get_type_id() != other.get_type_id()) {
            return static_cast<int>(get_type_id()) < static_cast<int>(other.get_type_id()) ? -1 : 1;
        }
        auto o = static_cast<const PowExpr*>(&other);
        int c = base_->compare(*o->base_);
        if (c != 0) return c;
        return exp_->compare(*o->exp_);
    }

    TypeId PowExpr::get_type_id() const {
        return TypeId::POW;
    }

    bool PowExpr::is_atomic() const { return false; }

    ExprPtr PowExpr::subs(const Substitution& subst) const {
        auto nb = std::dynamic_pointer_cast<const core::ArithmeticExpr>(base_->subs(subst));
        auto ne = std::dynamic_pointer_cast<const core::ArithmeticExpr>(exp_->subs(subst));
        if (!nb || !ne) throw std::runtime_error("subs: hijos no aritméticos");
        return std::make_shared<PowExpr>(nb, ne)->simplify();
    }

    ExprPtr PowExpr::simplify() const {
        return simplify_impl();
    }

    ArithmeticExprPtr PowExpr::simplify_impl() const {
        // 1. Simplificar hijos
        auto base = base_->simplify_arith();
        auto exp  = exp_->simplify_arith();

        // 2. x^0 = 1
        if (exp->is_zero()) return make_number(1);

        // 3. x^1 = x
        if (exp->is_one()) return base;

        // 4. 0^x = 0 (asumiendo x > 0; para CAS básico es aceptable)
        if (base->is_zero()) return make_number(0);

        // 5. 1^x = 1
        if (base->is_one()) return make_number(1);

        // 6. numérico ^ numérico
        if (base->is_numeric() && exp->is_numeric()) {
            double b = base->to_double();
            double e = exp->to_double();

            // Exponente entero no negativo → cálculo exacto por multiplicación repetida
            if (e == std::floor(e) && e >= 0 && e < 1000) {
                double r = 1.0;
                for (int i = 0; i < static_cast<int>(e); ++i) r *= b;
                return make_number(r);
            }
            // Exponente entero negativo → 1 / b^(-e)
            if (e == std::floor(e) && e < 0 && e > -1000) {
                double r = 1.0;
                for (int i = 0; i < static_cast<int>(-e); ++i) r *= b;
                if (r == 0.0) throw std::runtime_error("División por cero en Pow");
                return make_number(1.0 / r);
            }
            // Base positiva → pow estándar
            if (b > 0.0) {
                return make_number(std::pow(b, e));
            }
            // Base negativa con exponente no entero: dejamos simbólico (resultado complejo)
        }

        // 7. (x^a)^b = x^(a*b) si b es entero
        if (auto inner = std::dynamic_pointer_cast<const PowExpr>(base)) {
            bool b_es_entero = exp->is_numeric() &&
                               exp->to_double() == std::floor(exp->to_double());
            if (b_es_entero) {
                auto new_exp = inner->get_exp()->mul(exp)->simplify_arith();
                return std::make_shared<PowExpr>(inner->get_base(), new_exp)
                       ->simplify_impl();
            }
        }

        // 8. Sin cambios: reconstruimos con hijos simplificados
        return std::make_shared<PowExpr>(base, exp);
    }

    // ------------------------------------------------------------
    // Operaciones aritméticas
    // ------------------------------------------------------------

    ArithmeticExprPtr PowExpr::add(const ArithmeticExprPtr& other) const {
        std::vector<ArithmeticExprPtr> terms;
        terms.push_back(shared_arith());
        terms.push_back(other);
        return std::make_shared<AddExpr>(std::move(terms))->simplify_arith();
    }

    ArithmeticExprPtr PowExpr::mul(const ArithmeticExprPtr& other) const {
        std::vector<ArithmeticExprPtr> factors;
        factors.push_back(shared_arith());
        factors.push_back(other);
        return std::make_shared<MulExpr>(std::move(factors))->simplify_arith();
    }

    ArithmeticExprPtr PowExpr::pow(const ArithmeticExprPtr& other) const {
        return std::make_shared<PowExpr>(shared_arith(), other)->simplify_arith();
    }

    ArithmeticExprPtr PowExpr::neg() const {
        std::vector<ArithmeticExprPtr> factors;
        factors.push_back(make_number(-1));
        factors.push_back(shared_arith());
        return std::make_shared<MulExpr>(std::move(factors));
    }

    // ------------------------------------------------------------
    // Derivada
    // (u^v)' = u^v * (v' * ln(u) + v * u'/u)
    // Si v' = 0: (u^v)' = v * u^(v-1) * u'
    // ------------------------------------------------------------

    ArithmeticExprPtr PowExpr::diff(const VariableExprPtr& var) const {
        auto u_prime = base_->diff(var);
        auto v_prime = exp_->diff(var);

        // Caso 1: exponente constante (v' = 0)
        if (v_prime->is_zero()) {
            auto v_menos_1 = exp_->add(make_number(-1))->simplify_arith();
            auto u_pow     = std::make_shared<PowExpr>(base_, v_menos_1)->simplify_arith();
            return exp_->mul(u_pow)->mul(u_prime)->simplify_arith();
        }

        throw std::runtime_error(
            "Derivada de u^v con v dependiente de la variable requiere ln(u) "
            "(no implementado aún)");

        // auto ln_u = std::make_shared<LnExpr>(base_);
        // auto term1 = v_prime->mul(ln_u);
        // auto term2 = exp_->mul(u_prime)->mul(std::make_shared<PowExpr>(base_, make_number(-1)));
        // auto bracket = term1->add(term2);
        // return shared_arith()->mul(bracket);
    }

    // ------------------------------------------------------------
    // Predicados
    // ------------------------------------------------------------

    bool PowExpr::is_zero()     const { return false; }
    bool PowExpr::is_one()      const { return base_->is_one(); }
    bool PowExpr::is_negative() const { return false; }
    bool PowExpr::is_positive() const { return true; }
    bool PowExpr::is_numeric()  const {
        return base_->is_numeric() && exp_->is_numeric();
    }

    double PowExpr::to_double() const {
        return std::pow(base_->to_double(), exp_->to_double());
    }

} // namespace heza::algebra