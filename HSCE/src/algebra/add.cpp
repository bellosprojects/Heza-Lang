#include <heza/algebra/add.hpp>
#include <heza/algebra/mul.hpp>
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
    // Constructor: aplana Add anidados al vuelo
    // ============================================================

    AddExpr::AddExpr(std::vector<ArithmeticExprPtr> terms) {
        for (auto& t : terms) {
            if (auto sub = std::dynamic_pointer_cast<const AddExpr>(t)) {
                terms_.insert(terms_.end(),
                              sub->terms_.begin(),
                              sub->terms_.end());
            } else {
                terms_.push_back(std::move(t));
            }
        }
    }

    // ============================================================
    // Representación
    // ============================================================

    std::string AddExpr::to_str() const {
        if (terms_.empty()) return "0";
        std::string r = terms_[0]->to_str();
        for (size_t i = 1; i < terms_.size(); ++i) {
            r += " + ";
            r += terms_[i]->to_str();
        }
        return r;
    }

    std::u32string AddExpr::to_latex() const {
        if (terms_.empty()) return U"0";
        std::u32string r = terms_[0]->to_latex();
        for (size_t i = 1; i < terms_.size(); ++i) {
            r += U" + ";
            r += terms_[i]->to_latex();
        }
        return r;
    }

    // ============================================================
    // Comparación / hash / clonación
    // ============================================================

    bool AddExpr::is_equal(const ExprPtr& other) const {
        auto o = std::dynamic_pointer_cast<const AddExpr>(other);
        if (!o) return false;
        if (terms_.size() != o->terms_.size()) return false;
        for (size_t i = 0; i < terms_.size(); ++i) {
            if (!terms_[i]->is_equal(o->terms_[i])) return false;
        }
        return true;
    }

    ExprPtr AddExpr::clone() const {
        return std::make_shared<AddExpr>(terms_);
    }

    size_t AddExpr::hash() const {
        size_t h = std::hash<int>{}(static_cast<int>(get_type_id()));
        for (auto& t : terms_) {
            h ^= t->hash() + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        }
        return h;
    }

    int AddExpr::compare(const core::Expr& other) const {
        if (get_type_id() != other.get_type_id()) {
            return static_cast<int>(get_type_id()) < static_cast<int>(other.get_type_id()) ? -1 : 1;
        }
        auto o = static_cast<const AddExpr*>(&other);
        size_t n = std::min(terms_.size(), o->terms_.size());
        for (size_t i = 0; i < n; ++i) {
            int c = terms_[i]->compare(*o->terms_[i]);
            if (c != 0) return c;
        }
        if (terms_.size() < o->terms_.size()) return -1;
        if (terms_.size() > o->terms_.size()) return  1;
        return 0;
    }

    TypeId AddExpr::get_type_id() const { return TypeId::ADD; }

    bool AddExpr::is_atomic() const { return false; }

    // ============================================================
    // Sustitución
    // ============================================================

    ExprPtr AddExpr::subs(const Substitution& subst) const {
        std::vector<ArithmeticExprPtr> new_terms;
        for (auto& t : terms_) {
            auto nt = std::dynamic_pointer_cast<const core::ArithmeticExpr>(t->subs(subst));
            if (!nt) throw std::runtime_error("Add::subs: término no aritmético");
            new_terms.push_back(nt);
        }
        return std::make_shared<AddExpr>(std::move(new_terms))->simplify();
    }

    // ============================================================
    // Helper: descomponer término en (coeficiente, parte simbólica)
    // ============================================================

    std::pair<double, ArithmeticExprPtr>
    AddExpr::extract_coefficient(const ArithmeticExprPtr& term) {
        // Caso 1: número puro → (valor, 1)
        if (term->is_numeric()) {
            return {term->to_double(), make_number(1)};
        }
        // Caso 2: producto → buscar el factor numérico
        if (auto mul = std::dynamic_pointer_cast<const MulExpr>(term)) {
            double coef = 1.0;
            std::vector<ArithmeticExprPtr> symbolic;
            for (auto& f : mul->get_factors()) {
                if (f->is_numeric()) coef *= f->to_double();
                else                 symbolic.push_back(f);
            }
            if (symbolic.empty())     return {coef, make_number(1)};
            if (symbolic.size() == 1) return {coef, symbolic[0]};
            return {coef, std::make_shared<MulExpr>(std::move(symbolic))};
        }
        // Caso 3: cualquier otra cosa → (1, term)
        return {1.0, term};
    }

    ArithmeticExprPtr
    AddExpr::build_term(double coef, const ArithmeticExprPtr& rest) {
        if (rest->is_one()) return make_number(coef);
        if (coef == 1.0)    return rest;
        std::vector<ArithmeticExprPtr> factors;
        factors.push_back(make_number(coef));
        factors.push_back(rest);
        return std::make_shared<MulExpr>(std::move(factors));
    }

    // ============================================================
    // simplify()
    // ============================================================

    ExprPtr AddExpr::simplify() const { return simplify_impl(); }

    ArithmeticExprPtr AddExpr::simplify_impl() const {
        // 1. Simplificar cada término y aplanar de nuevo
        std::vector<ArithmeticExprPtr> flat;
        for (auto& t : terms_) {
            auto s = t->simplify_arith();
            if (auto sub = std::dynamic_pointer_cast<const AddExpr>(s)) {
                flat.insert(flat.end(), sub->terms_.begin(), sub->terms_.end());
            } else {
                flat.push_back(s);
            }
        }

        // 2. Extraer (coef, rest), descartando ceros
        std::vector<std::pair<double, ArithmeticExprPtr>> pairs;
        for (auto& t : flat) {
            if (t->is_zero()) continue;
            pairs.push_back(extract_coefficient(t));
        }

        // 3. Agrupar términos semejantes (O(n²), aceptable para árboles típicos)
        std::vector<std::pair<double, ArithmeticExprPtr>> grouped;
        for (auto& p : pairs) {
            bool found = false;
            for (auto& g : grouped) {
                if (g.second->is_equal(p.second)) {
                    g.first += p.first;
                    found = true;
                    break;
                }
            }
            if (!found) grouped.push_back(p);
        }

        // 4. Reconstruir términos con coeficiente no nulo
        std::vector<ArithmeticExprPtr> result;
        for (auto& g : grouped) {
            if (g.first == 0.0) continue;
            result.push_back(build_term(g.first, g.second));
        }

        // 5. Ordenar canónicamente
        std::sort(result.begin(), result.end(),
                  [](const ArithmeticExprPtr& a, const ArithmeticExprPtr& b) {
                      return a->compare(*b) < 0;
                  });

        // 6. Colapsar
        if (result.empty())     return make_number(0);
        if (result.size() == 1) return result[0];
        return std::make_shared<AddExpr>(std::move(result));
    }

    // ============================================================
    // Operaciones aritméticas
    // ============================================================

    ArithmeticExprPtr AddExpr::add(const ArithmeticExprPtr& other) const {
        std::vector<ArithmeticExprPtr> new_terms(terms_);
        new_terms.push_back(other);
        return std::make_shared<AddExpr>(std::move(new_terms))->simplify_arith();
    }

    ArithmeticExprPtr AddExpr::mul(const ArithmeticExprPtr& other) const {
        std::vector<ArithmeticExprPtr> factors;
        factors.push_back(shared_arith());
        factors.push_back(other);
        return std::make_shared<MulExpr>(std::move(factors))->simplify_arith();
    }

    ArithmeticExprPtr AddExpr::pow(const ArithmeticExprPtr& other) const {
        return std::make_shared<PowExpr>(shared_arith(), other)->simplify_arith();
    }

    ArithmeticExprPtr AddExpr::neg() const {
        std::vector<ArithmeticExprPtr> factors;
        factors.push_back(make_number(-1));
        factors.push_back(shared_arith());
        return std::make_shared<MulExpr>(std::move(factors))->simplify_arith();
    }

    // ============================================================
    // Derivada: (f + g + h)' = f' + g' + h'
    // ============================================================

    ArithmeticExprPtr AddExpr::diff(const VariableExprPtr& var) const {
        std::vector<ArithmeticExprPtr> derivs;
        for (auto& t : terms_) derivs.push_back(t->diff(var));
        return std::make_shared<AddExpr>(std::move(derivs))->simplify_arith();
    }

    // ============================================================
    // Predicados
    // ============================================================

    bool AddExpr::is_zero() const {
        for (auto& t : terms_) if (!t->is_zero()) return false;
        return true;
    }

    bool AddExpr::is_one() const { return false; }

    bool AddExpr::is_negative() const {
        if (terms_.empty()) return false;
        for (auto& t : terms_) if (!t->is_negative()) return false;
        return true;
    }

    bool AddExpr::is_positive() const {
        if (terms_.empty()) return false;
        for (auto& t : terms_) if (!t->is_positive()) return false;
        return true;
    }

    bool AddExpr::is_numeric() const {
        for (auto& t : terms_) if (!t->is_numeric()) return false;
        return true;
    }

    double AddExpr::to_double() const {
        double s = 0.0;
        for (auto& t : terms_) s += t->to_double();
        return s;
    }

} // namespace heza::algebra