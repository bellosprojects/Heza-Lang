#include <heza/algebra/mul.hpp>
#include <heza/algebra/add.hpp>
#include <heza/algebra/pow.hpp>
#include <heza/atomic/number.hpp>
#include <heza/atomic/variable.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace heza::algebra {

    using core::ArithmeticExprPtr;
    using core::ExprPtr;
    using core::Substitution;
    using core::TypeId;
    using atomic::make_number;
    using atomic::VariableExprPtr;

    // ============================================================
    // Constructor: aplana Mul anidados al vuelo
    // ============================================================

    MulExpr::MulExpr(std::vector<ArithmeticExprPtr> factors) {
        for (auto& f : factors) {
            if (auto sub = std::dynamic_pointer_cast<const MulExpr>(f)) {
                factors_.insert(factors_.end(),
                                sub->factors_.begin(),
                                sub->factors_.end());
            } else {
                factors_.push_back(std::move(f));
            }
        }
    }

    // ============================================================
    // Representación
    // ============================================================

    std::string MulExpr::to_str() const {
        if (factors_.empty()) return "1";
        std::string r = factors_[0]->to_str();
        for (size_t i = 1; i < factors_.size(); ++i) {
            r += " * ";
            r += factors_[i]->to_str();
        }
        return r;
    }

    std::u32string MulExpr::to_latex() const {
        if (factors_.empty()) return U"1";
        std::u32string r = factors_[0]->to_latex();
        for (size_t i = 1; i < factors_.size(); ++i) {
            // Para factores no numéricos, usar \cdot en lugar de yuxtaposición
            // evita ambigüedades como "23" (que significa veintitrés, no 2·3).
            bool prev_num = factors_[i-1]->is_numeric();
            bool curr_num = factors_[i]->is_numeric();
            if (prev_num && curr_num) {
                r += U" \\cdot ";
            } else {
                r += U"\\,";  // espacio fino (yuxtaposición implícita)
            }
            r += factors_[i]->to_latex();
        }
        return r;
    }

    // ============================================================
    // Comparación / hash / clonación
    // ============================================================

    bool MulExpr::is_equal(const ExprPtr& other) const {
        auto o = std::dynamic_pointer_cast<const MulExpr>(other);
        if (!o) return false;
        if (factors_.size() != o->factors_.size()) return false;
        for (size_t i = 0; i < factors_.size(); ++i) {
            if (!factors_[i]->is_equal(o->factors_[i])) return false;
        }
        return true;
    }

    ExprPtr MulExpr::clone() const {
        return std::make_shared<MulExpr>(factors_);
    }

    size_t MulExpr::hash() const {
        size_t h = std::hash<int>{}(static_cast<int>(get_type_id()));
        for (auto& f : factors_) {
            h ^= f->hash() + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        }
        return h;
    }

    int MulExpr::compare(const core::Expr& other) const {
        if (get_type_id() != other.get_type_id()) {
            return static_cast<int>(get_type_id()) < static_cast<int>(other.get_type_id()) ? -1 : 1;
        }
        auto o = static_cast<const MulExpr*>(&other);
        size_t n = std::min(factors_.size(), o->factors_.size());
        for (size_t i = 0; i < n; ++i) {
            int c = factors_[i]->compare(*o->factors_[i]);
            if (c != 0) return c;
        }
        if (factors_.size() < o->factors_.size()) return -1;
        if (factors_.size() > o->factors_.size()) return  1;
        return 0;
    }

    TypeId MulExpr::get_type_id() const { return TypeId::MUL; }

    bool MulExpr::is_atomic() const { return false; }

    // ============================================================
    // Sustitución
    // ============================================================

    ExprPtr MulExpr::subs(const Substitution& subst) const {
        std::vector<ArithmeticExprPtr> new_factors;
        for (auto& f : factors_) {
            auto nf = std::dynamic_pointer_cast<const core::ArithmeticExpr>(f->subs(subst));
            if (!nf) throw std::runtime_error("Mul::subs: factor no aritmético");
            new_factors.push_back(nf);
        }
        return std::make_shared<MulExpr>(std::move(new_factors))->simplify();
    }

    // ============================================================
    // Helper: descomponer factor en (base, exponente)
    // ============================================================

    std::pair<ArithmeticExprPtr, ArithmeticExprPtr>
    MulExpr::extract_base_exp(const ArithmeticExprPtr& factor) {
        if (auto p = std::dynamic_pointer_cast<const PowExpr>(factor)) {
            return {p->get_base(), p->get_exp()};
        }
        return {factor, make_number(1)};
    }

    ArithmeticExprPtr
    MulExpr::build_factor(const ArithmeticExprPtr& base,
                          const ArithmeticExprPtr& exp) {
        if (exp->is_zero()) return make_number(1);
        if (exp->is_one())  return base;
        return std::make_shared<PowExpr>(base, exp)->simplify_arith();
    }

    // ============================================================
    // simplify()
    // ============================================================

    ExprPtr MulExpr::simplify() const { return simplify_impl(); }

    ArithmeticExprPtr MulExpr::simplify_impl() const {
        // 1. Simplificar cada factor y aplanar
        std::vector<ArithmeticExprPtr> flat;
        for (auto& f : factors_) {
            auto s = f->simplify_arith();
            if (auto sub = std::dynamic_pointer_cast<const MulExpr>(s)) {
                flat.insert(flat.end(), sub->factors_.begin(), sub->factors_.end());
            } else {
                flat.push_back(s);
            }
        }

        // 2. Regla de absorción: si algún factor es 0 → 0
        for (auto& f : flat) if (f->is_zero()) return make_number(0);

        // 3. Separar coeficiente numérico, descartar unos
        double coef = 1.0;
        std::vector<ArithmeticExprPtr> symbolic;
        for (auto& f : flat) {
            if (f->is_numeric()) coef *= f->to_double();
            else if (f->is_one()) continue;               // neutro: se ignora
            else symbolic.push_back(f);
        }

        // 4. Agrupar por base, sumando exponentes (O(n²) por simplicidad)
        std::vector<std::pair<ArithmeticExprPtr, ArithmeticExprPtr>> grouped;
        for (auto& f : symbolic) {
            auto [base, exp] = extract_base_exp(f);
            bool found = false;
            for (auto& g : grouped) {
                if (g.first->is_equal(base)) {
                    g.second = g.second->add(exp)->simplify_arith();
                    found = true;
                    break;
                }
            }
            if (!found) grouped.emplace_back(base, exp);
        }

        // 5. Reconstruir factores (descartando los que quedan con exp 0)
        std::vector<ArithmeticExprPtr> rebuilt;
        for (auto& g : grouped) {
            if (g.second->is_zero()) continue;            // base^0 = 1, se ignora
            rebuilt.push_back(build_factor(g.first, g.second));
        }

        // 6. Ordenar canónicamente
        std::sort(rebuilt.begin(), rebuilt.end(),
                  [](const ArithmeticExprPtr& a, const ArithmeticExprPtr& b) {
                      return a->compare(*b) < 0;
                  });

        // 7. Colapsar
        if (coef == 0.0) return make_number(0);

        // Sin parte simbólica → solo el número
        if (rebuilt.empty()) return make_number(coef);

        // Coeficiente 1 y solo simbólicos → colapsar si es único
        if (coef == 1.0) {
            if (rebuilt.size() == 1) return rebuilt[0];
            return std::make_shared<MulExpr>(std::move(rebuilt));
        }

        // Coeficiente ≠ 1 → insertar al frente
        rebuilt.insert(rebuilt.begin(), make_number(coef));
        if (rebuilt.size() == 1) return rebuilt[0];       // (nunca pasa aquí)
        return std::make_shared<MulExpr>(std::move(rebuilt));
    }

    // ============================================================
    // Operaciones aritméticas
    // ============================================================

    ArithmeticExprPtr MulExpr::add(const ArithmeticExprPtr& other) const {
        std::vector<ArithmeticExprPtr> terms;
        terms.push_back(shared_arith());
        terms.push_back(other);
        return std::make_shared<AddExpr>(std::move(terms))->simplify_arith();
    }

    ArithmeticExprPtr MulExpr::mul(const ArithmeticExprPtr& other) const {
        std::vector<ArithmeticExprPtr> new_factors(factors_);
        new_factors.push_back(other);
        return std::make_shared<MulExpr>(std::move(new_factors))->simplify_arith();
    }

    ArithmeticExprPtr MulExpr::pow(const ArithmeticExprPtr& other) const {
        return std::make_shared<PowExpr>(shared_arith(), other)->simplify_arith();
    }

    ArithmeticExprPtr MulExpr::neg() const {
        std::vector<ArithmeticExprPtr> new_factors;
        new_factors.push_back(make_number(-1));
        new_factors.push_back(shared_arith());
        return std::make_shared<MulExpr>(std::move(new_factors))->simplify_arith();
    }

    // ============================================================
    // Derivada: regla del producto para n factores
    //   (f1 * f2 * ... * fn)' = Σ_i (f1 * ... * fi' * ... * fn)
    // ============================================================

    ArithmeticExprPtr MulExpr::diff(const VariableExprPtr& var) const {
        std::vector<ArithmeticExprPtr> sum_terms;
        for (size_t i = 0; i < factors_.size(); ++i) {
            auto d = factors_[i]->diff(var);
            if (d->is_zero()) continue;                   // este término no aporta
            std::vector<ArithmeticExprPtr> product(factors_);
            product[i] = d;
            sum_terms.push_back(
                std::make_shared<MulExpr>(std::move(product))->simplify_arith());
        }
        if (sum_terms.empty()) return make_number(0);
        if (sum_terms.size() == 1) return sum_terms[0];
        return std::make_shared<AddExpr>(std::move(sum_terms))->simplify_arith();
    }

    // ============================================================
    // Predicados
    // ============================================================

    bool MulExpr::is_zero() const {
        for (auto& f : factors_) if (f->is_zero()) return true;
        return false;
    }

    bool MulExpr::is_one() const {
        for (auto& f : factors_) if (!f->is_one()) return false;
        return true;
    }

    bool MulExpr::is_negative() const {
        // Negativo si hay una cantidad impar de factores negativos
        int neg_count = 0;
        for (auto& f : factors_) {
            if (f->is_negative()) ++neg_count;
            else if (!f->is_positive()) return false;     // factor de signo desconocido
        }
        return (neg_count % 2) == 1;
    }

    bool MulExpr::is_positive() const {
        for (auto& f : factors_) if (!f->is_positive()) return false;
        return true;
    }

    bool MulExpr::is_numeric() const {
        for (auto& f : factors_) if (!f->is_numeric()) return false;
        return true;
    }

    double MulExpr::to_double() const {
        double p = 1.0;
        for (auto& f : factors_) p *= f->to_double();
        return p;
    }

} // namespace heza::algebra