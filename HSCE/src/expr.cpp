#include "expr.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

auto one = make_number(1.0);
auto zero = make_number(0.0);
auto two = make_number(2.0);
auto m_one = make_number(-1.0);

std::complex<double> extract_complex(const ExprPtr& e) {
    if (auto num = std::dynamic_pointer_cast<Number>(e)) {
        return {num->value(), 0.0}; 
    }
    if (auto i_unit = std::dynamic_pointer_cast<ImaginaryUnit>(e)) {
        return {0.0, 1.0};
    }
    if (auto add = std::dynamic_pointer_cast<Add>(e)) {
        return extract_complex(add->left_) + extract_complex(add->right_);
    }
    if (auto sub = std::dynamic_pointer_cast<Sub>(e)) {
        return extract_complex(sub->left_) - extract_complex(sub->right_);
    }
    if (auto mul = std::dynamic_pointer_cast<Mul>(e)) {
        return extract_complex(mul->left_) * extract_complex(mul->right_);
    }
    if (auto div = std::dynamic_pointer_cast<Div>(e)) {
        return extract_complex(div->left_) / extract_complex(div->right_);
    }
    if (auto pow = std::dynamic_pointer_cast<Pow>(e)) {
        return std::pow(extract_complex(pow->left_), extract_complex(pow->right_));
    }
    
    throw std::runtime_error("The expression cannot be evaluated numerically; it contains variables or unresolved functions.");
}



ExprPtr factorize(const ExprPtr& expr){

    static std::vector<RewriteRule> factorize_rules = {
        // 1. X + X^2 = X * (X + 1)
        RewriteRule(
            std::make_shared<Add>(
                std::make_shared<Wildcard>("X"),
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0))
            ),
            std::make_shared<Mul>(
                std::make_shared<Wildcard>("X"),
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), make_number(1.0))
            ) 
        ),

        // 2. X^2 - Y^2 = (X + Y) * (X - Y)
        RewriteRule(
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                std::make_shared<Mul>(
                    make_number(-1.0),
                    std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                )
            ),
            std::make_shared<Mul>(
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                std::make_shared<Add>(
                    std::make_shared<Wildcard>("X"),
                    std::make_shared<Mul>(make_number(-1.0), std::make_shared<Wildcard>("Y"))
                )
            )
        ),

        // 3. Trinomio Cuadrado Perfecto (+): X^2 + 2*X*Y + Y^2 = (X + Y)^2
        RewriteRule(
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                std::make_shared<Add>(
                    std::make_shared<Mul>(
                        make_number(2.0),
                        std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y"))
                    ),
                    std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                )
            ),
            std::make_shared<Pow>(
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                make_number(2.0)
            )
        ),

        // 4. Trinomio Cuadrado Perfecto (-): X^2 - 2*X*Y + Y^2 = (X - Y)^2
        RewriteRule(
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                std::make_shared<Add>(
                    std::make_shared<Mul>(
                        make_number(-2.0),
                        std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y"))
                    ),
                    std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                )
            ),
            std::make_shared<Pow>(
                std::make_shared<Add>(
                    std::make_shared<Wildcard>("X"),
                    std::make_shared<Mul>(make_number(-1.0), std::make_shared<Wildcard>("Y"))
                ),
                make_number(2.0)
            )
        ),

        // 5. Factor común distributivo: X*Y + X*Z = X * (Y + Z)
        RewriteRule(
            std::make_shared<Add>(
                std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Z"))
            ),
            std::make_shared<Mul>(
                std::make_shared<Wildcard>("X"),
                std::make_shared<Add>(std::make_shared<Wildcard>("Y"), std::make_shared<Wildcard>("Z"))
            )
        ),

        // 6. Suma de Cubos: X^3 + Y^3 = (X + Y) * (X^2 - X*Y + Y^2)
        RewriteRule(
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(3.0)),
                std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(3.0))
            ),
            std::make_shared<Mul>(
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                std::make_shared<Add>(
                    std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                    std::make_shared<Add>(
                        std::make_shared<Mul>(
                            make_number(-1.0),
                            std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y"))
                        ),
                        std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                    )
                )
            )
        )
    };

    static RewriteEngine factor_engine(factorize_rules);

    return factor_engine.simplify(expr);
}

ExprPtr expand(const ExprPtr& expr){

    static std::vector<RewriteRule> factorize_rules = {
        // 6. Suma de Cubos: X^3 + Y^3 = (X + Y) * (X^2 - X*Y + Y^2)
        RewriteRule(
            std::make_shared<Mul>(
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                std::make_shared<Add>(
                    std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                    std::make_shared<Add>(
                        std::make_shared<Mul>(
                            make_number(-1.0),
                            std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y"))
                        ),
                        std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                    )
                )
            ),
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(3.0)),
                std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(3.0))
            )
        ),
        
        // 1. X + X^2 = X * (X + 1)
        RewriteRule(
            std::make_shared<Mul>(
                std::make_shared<Wildcard>("X"),
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), make_number(1.0))
            ),
            std::make_shared<Add>(
                std::make_shared<Wildcard>("X"),
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0))
            )
        ),

        // 2. X^2 - Y^2 = (X + Y) * (X - Y)
        RewriteRule(
            std::make_shared<Mul>(
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                std::make_shared<Add>(
                    std::make_shared<Wildcard>("X"),
                    std::make_shared<Mul>(make_number(-1.0), std::make_shared<Wildcard>("Y"))
                )
            ),
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                std::make_shared<Mul>(
                    make_number(-1.0),
                    std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                )
            )
        ),

        // 3. Trinomio Cuadrado Perfecto (+): X^2 + 2*X*Y + Y^2 = (X + Y)^2
        RewriteRule(
            std::make_shared<Pow>(
                std::make_shared<Add>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                make_number(2.0)
            ),
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                std::make_shared<Add>(
                    std::make_shared<Mul>(
                        make_number(2.0),
                        std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y"))
                    ),
                    std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                )
            )
        ),

        // 4. Trinomio Cuadrado Perfecto (-): X^2 - 2*X*Y + Y^2 = (X - Y)^2
        RewriteRule(
            std::make_shared<Pow>(
                std::make_shared<Add>(
                    std::make_shared<Wildcard>("X"),
                    std::make_shared<Mul>(make_number(-1.0), std::make_shared<Wildcard>("Y"))
                ),
                make_number(2.0)
            ),
            std::make_shared<Add>(
                std::make_shared<Pow>(std::make_shared<Wildcard>("X"), make_number(2.0)),
                std::make_shared<Add>(
                    std::make_shared<Mul>(
                        make_number(-2.0),
                        std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y"))
                    ),
                    std::make_shared<Pow>(std::make_shared<Wildcard>("Y"), make_number(2.0))
                )
            )
        ),

        // 5. Factor común distributivo: X*Y + X*Z = X * (Y + Z)
        RewriteRule(
            std::make_shared<Mul>(
                std::make_shared<Wildcard>("X"),
                std::make_shared<Add>(std::make_shared<Wildcard>("Y"), std::make_shared<Wildcard>("Z"))
            ),
            std::make_shared<Add>(
                std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Y")),
                std::make_shared<Mul>(std::make_shared<Wildcard>("X"), std::make_shared<Wildcard>("Z"))
            )
        )
    };

    static RewriteEngine factor_engine(factorize_rules);

    return factor_engine.simplify(expr);
}

void flatten_add(const ExprPtr&e, std::vector<ExprPtr>&terms){
    if(auto add = std::dynamic_pointer_cast<Add>(e)){
        flatten_add(add->left_, terms);
        flatten_add(add->right_, terms);
    } else {
        terms.push_back(e->normalize());
    }
}

void flatten_mul(const ExprPtr&e, std::vector<ExprPtr>&factors){
    if(auto mul = std::dynamic_pointer_cast<Mul>(e)){
        flatten_mul(mul->left_, factors);
        flatten_mul(mul->right_, factors);
    } else {
        factors.push_back(e->normalize());
    }
}

ExprPtr RewriteRule::apply(const ExprPtr& expr) const {
    // 1. Intento principal en la raíz exacta
    Substitution env;
    if (pattern_->match(expr, env)) {
        return replacement_->substitute(env);
    }

    // 2. Subset matching inteligente por Anclas para Add
    auto expr_add = std::dynamic_pointer_cast<Add>(expr);
    auto patt_add = std::dynamic_pointer_cast<Add>(pattern_);
    if (patt_add && expr_add) {
        std::vector<ExprPtr> p_terms;
        std::vector<ExprPtr> e_terms;
        flatten_add(pattern_, p_terms);
        flatten_add(expr, e_terms);

        // Usamos cada término del patrón como un posible "ancla" inicial
        for (const auto& anchor_p : p_terms) {
            for (const auto& anchor_e : e_terms) {
                Substitution base_env;
                
                // Si el ancla hace match, descubrimos algunos comodines (ej. X = 1 + y)
                if (anchor_p->match(anchor_e, base_env)) {
                    
                    // Sustituimos lo descubierto en el patrón y lo aplanamos
                    ExprPtr inst_pattern = pattern_->substitute(base_env);
                    std::vector<ExprPtr> inst_p_terms;
                    flatten_add(inst_pattern, inst_p_terms);

                    // Ahora comparamos las listas aplanadas
                    if (inst_p_terms.size() <= e_terms.size()) {
                        Substitution full_env = base_env;
                        std::vector<bool> used(e_terms.size(), false);
                        bool all_found = true;

                        for (const auto& ip_term : inst_p_terms) {
                            bool found = false;
                            for (size_t i = 0; i < e_terms.size(); ++i) {
                                if (used[i]) continue;
                                Substitution trial_env = full_env;
                                // Seguimos recolectando posibles comodines sobrantes
                                if (ip_term->match(e_terms[i], trial_env)) {
                                    full_env = trial_env;
                                    used[i] = true;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                all_found = false;
                                break;
                            }
                        }

                        // ¡Si todos los términos del patrón instanciado se encontraron!
                        if (all_found) {
                            ExprPtr replaced = replacement_->substitute(full_env);
                            std::vector<ExprPtr> leftovers;
                            for (size_t i = 0; i < e_terms.size(); ++i) {
                                if (!used[i]) leftovers.push_back(e_terms[i]);
                            }

                            if (leftovers.empty()) return replaced;
                            ExprPtr result = replaced;
                            for (const auto& lo : leftovers) {
                                result = std::make_shared<Add>(result, lo);
                            }
                            return result;
                        }
                    }
                }
            }
        }
    }

    // 3. Subset matching inteligente por Anclas para Mul
    auto expr_mul = std::dynamic_pointer_cast<Mul>(expr);
    auto patt_mul = std::dynamic_pointer_cast<Mul>(pattern_);
    if (patt_mul && expr_mul) {
        std::vector<ExprPtr> p_factors;
        std::vector<ExprPtr> e_factors;
        flatten_mul(pattern_, p_factors);
        flatten_mul(expr, e_factors);

        for (const auto& anchor_p : p_factors) {
            for (const auto& anchor_e : e_factors) {
                Substitution base_env;
                if (anchor_p->match(anchor_e, base_env)) {
                    
                    ExprPtr inst_pattern = pattern_->substitute(base_env);
                    std::vector<ExprPtr> inst_p_factors;
                    flatten_mul(inst_pattern, inst_p_factors);

                    if (inst_p_factors.size() <= e_factors.size()) {
                        Substitution full_env = base_env;
                        std::vector<bool> used(e_factors.size(), false);
                        bool all_found = true;

                        for (const auto& ip_factor : inst_p_factors) {
                            bool found = false;
                            for (size_t i = 0; i < e_factors.size(); ++i) {
                                if (used[i]) continue;
                                Substitution trial_env = full_env;
                                if (ip_factor->match(e_factors[i], trial_env)) {
                                    full_env = trial_env;
                                    used[i] = true;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                all_found = false;
                                break;
                            }
                        }

                        if (all_found) {
                            ExprPtr replaced = replacement_->substitute(full_env);
                            std::vector<ExprPtr> leftovers;
                            for (size_t i = 0; i < e_factors.size(); ++i) {
                                if (!used[i]) leftovers.push_back(e_factors[i]);
                            }

                            if (leftovers.empty()) return replaced;
                            ExprPtr result = replaced;
                            for (const auto& lo : leftovers) {
                                result = std::make_shared<Mul>(result, lo);
                            }
                            return result;
                        }
                    }
                }
            }
        }
    }

    // 4. Fallback: Buscar recursivamente en los hijos del árbol
    ExprPtr result = nullptr;
    expr->transform_children([this, &result](const ExprPtr& child) {
        if (!result) {
            result = this->apply(child);
        }
        return child;
    });

    return result;
}

bool is_zero(const ExprPtr& e) {
    if (auto n = std::dynamic_pointer_cast<Number>(e)) return n->value() == 0.0;
    return false;
}
bool is_one(const ExprPtr& e) { 
    if (auto n = std::dynamic_pointer_cast<Number>(e)) return n->value() == 1.0;
    return false;
}
bool is_minus_one(const ExprPtr& e) {
    if (auto n = std::dynamic_pointer_cast<Number>(e)) return n->value() == -1.0;
    return false;
}
bool is_number(const ExprPtr& e, double& val) {
    if (auto n = std::dynamic_pointer_cast<Number>(e)) { val = n->value(); return true; }
    return false;
}

std::pair<double, ExprPtr> extract_coeff(const ExprPtr& e) {
    if (auto num = std::dynamic_pointer_cast<Number>(e)) {
        return {num->value(), std::make_shared<Number>(1.0)};
    }
    if (auto mul = std::dynamic_pointer_cast<Mul>(e)) {
        if (auto num = std::dynamic_pointer_cast<Number>(mul->left_)) 
            return {num->value(), mul->right_};
        if (auto num = std::dynamic_pointer_cast<Number>(mul->right_)) 
            return {num->value(), mul->left_};
    }
    return {1.0, e};
}

std::pair<ExprPtr, double> extract_exponent(const ExprPtr& e) {
    if (auto pow = std::dynamic_pointer_cast<Pow>(e)) {
        if (auto num = std::dynamic_pointer_cast<Number>(pow->right_)) {
            return {pow->left_, num->value()};
        }
    }
    return {e, 1.0};
}

// ---- Operadores binarios (definidos después de todas las clases) ----
ExprPtr Expr::add(const ExprPtr& other) { return std::make_shared<Add>(shared_from_this(), other); }
ExprPtr Expr::add_double(double v) { return std::make_shared<Add>(shared_from_this(), make_number(v)); }
ExprPtr Expr::mul(const ExprPtr& other) { return std::make_shared<Mul>(shared_from_this(), other); }
ExprPtr Expr::mul_double(double v) { return std::make_shared<Mul>(shared_from_this(), make_number(v)); }
ExprPtr Expr::sub(const ExprPtr& other) { return std::make_shared<Sub>(shared_from_this(), other); }
ExprPtr Expr::sub_double(double v) { return std::make_shared<Sub>(shared_from_this(), make_number(v)); }
ExprPtr Expr::rsub_double(double v) { return std::make_shared<Sub>(make_number(v), shared_from_this()); }
ExprPtr Expr::div(const ExprPtr& other) { return std::make_shared<Div>(shared_from_this(), other); }
ExprPtr Expr::div_double(double v) { return std::make_shared<Div>(shared_from_this(), make_number(v)); }
ExprPtr Expr::rdiv_double(double v) { return std::make_shared<Div>(make_number(v), shared_from_this()); }
ExprPtr Expr::pow(const ExprPtr& other) { return std::make_shared<Pow>(shared_from_this(), other); }
ExprPtr Expr::pow_double(double v) { return std::make_shared<Pow>(shared_from_this(), make_number(v)); }
ExprPtr Expr::rpow_double(double v) { return std::make_shared<Pow>(make_number(v),shared_from_this()); }


NumericValue Expr::number() {
    std::map<std::string, double> map;
    auto s_expr = evaluate(map)->simplify();
    
    std::complex<double> result = extract_complex(s_expr);

    if(std::abs(result.imag()) < 1e-12){
        return result.real();
    } else {
        return result;
    }
}

bool Expr::is_equal(const ExprPtr& other) const {
    if (!other) return false;
    auto norm_this = this->normalize();
    auto norm_other = other->normalize();
    return norm_this->compare(norm_other) == 0;
}
int Expr::compare(const ExprPtr& other) const {
    if(!other) return 1;
    int this_id = this->type_id();
    int other_id = other->type_id();
    if(this_id != other_id) {
        return (this_id < other_id)? -1 : 1;
    }
    return compare_same_type(other);
}
bool Expr::less(const ExprPtr& a, const ExprPtr& b) {
    if (!a && !b) return false;
    if(!a) return true;
    if(!b) return false;
    return a->compare(b) < 0;
}

int ImaginaryUnit::type_id() const { return -1; }
int Number::type_id() const { return 0; }
int Constant::type_id() const { return 1; }
int Infinite::type_id() const { return 2; }
int Variable::type_id() const { return 3; }

int Abs::type_id() const { return 10; }
int Sign::type_id() const { return 11; }
int Exp::type_id() const { return 12; }
int Log::type_id() const { return 13; }
int Sqrt::type_id() const { return 14; }

int Sin::type_id() const { return 15; }
int Cos::type_id() const { return 16; }
int Tan::type_id() const { return 17; }
int Sec::type_id() const { return 18; }
int Csc::type_id() const { return 19; }
int Cot::type_id() const { return 20; }
int Arcsin::type_id() const { return 21; }
int Arccos::type_id() const { return 22; }
int Arctan::type_id() const { return 23; }
int Arcsec::type_id() const { return 24; }
int Arccsc::type_id() const { return 25; }
int Arccot::type_id() const { return 26; }
int Sinh::type_id() const { return 27; }
int Cosh::type_id() const { return 28; }
int Tanh::type_id() const { return 29; }
int Sech::type_id() const { return 30; }
int Csch::type_id() const { return 31; }
int Coth::type_id() const { return 32; }
int Arcsinh::type_id() const { return 33; }
int Arccosh::type_id() const { return 34; }
int Arctanh::type_id() const { return 35; }
int Arccoth::type_id() const { return 36; }
int Arcsech::type_id() const { return 37; }
int Arccsch::type_id() const { return 38; }

int Pow::type_id() const { return 50; }
int Root::type_id() const { return 51; }
int Mul::type_id() const { return 52; }
int Div::type_id() const { return 53; }
int Add::type_id() const { return 54; }
int Sub::type_id() const { return 55; }
int LogBase::type_id() const { return 56; }

// -- Comparaciones del mismo tipo y normalizacion canonica
int Number::compare_same_type(const ExprPtr&other) const {
    auto o = std::static_pointer_cast<Number>(other);
    if (value_ < o->value()) return -1;
    if (value_ > o->value()) return 1;
    return 0;
} 
ExprPtr Number::normalize() const {
    return std::const_pointer_cast<Expr>(shared_from_this());
}

int Variable::compare_same_type(const ExprPtr&other) const {
    auto o = std::static_pointer_cast<Variable>(other);
    if (name_ < o->name_) return -1;
    if (name_ > o->name_) return 1;
    return 0;
} 
ExprPtr Variable::normalize() const {
    return std::const_pointer_cast<Expr>(shared_from_this());
}

int Constant::compare_same_type(const ExprPtr&other) const {
    auto o = std::static_pointer_cast<Constant>(other);
    if (name_ < o->name_) return -1;
    if (name_ > o->name_) return 1;
    return 0;
} 
ExprPtr Constant::normalize() const {
    return std::const_pointer_cast<Expr>(shared_from_this());
}

int Infinite::compare_same_type(const ExprPtr&other) const {
    return 0;
} 
ExprPtr Infinite::normalize() const {
    return std::const_pointer_cast<Expr>(shared_from_this());
}

int Sin::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sin>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Sin::normalize() const {
    return std::make_shared<Sin>(arg_->normalize());
}

int Cos::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Cos>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Cos::normalize() const {
    return std::make_shared<Cos>(arg_->normalize());
}

int Tan::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Tan>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Tan::normalize() const {
    return std::make_shared<Tan>(arg_->normalize());
}

int Sec::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sec>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Sec::normalize() const {
    return std::make_shared<Sec>(arg_->normalize());
}

int Csc::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Csc>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Csc::normalize() const {
    return std::make_shared<Csc>(arg_->normalize());
}

int Cot::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Cot>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Cot::normalize() const {
    return std::make_shared<Cot>(arg_->normalize());
}

int Arcsin::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arcsin>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arcsin::normalize() const {
    return std::make_shared<Arcsin>(arg_->normalize());
}

int Arccos::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arccos>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arccos::normalize() const {
    return std::make_shared<Arccos>(arg_->normalize());
}

int Arctan::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arctan>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arctan::normalize() const {
    return std::make_shared<Arctan>(arg_->normalize());
}

int Arcsec::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arcsec>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arcsec::normalize() const {
    return std::make_shared<Arcsec>(arg_->normalize());
}

int Arccsc::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arccsc>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arccsc::normalize() const {
    return std::make_shared<Arccsc>(arg_->normalize());
}

int Arccot::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arccot>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arccot::normalize() const {
    return std::make_shared<Arccot>(arg_->normalize());
}

int Sinh::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sinh>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Sinh::normalize() const {
    return std::make_shared<Sinh>(arg_->normalize());
}

int Cosh::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Cosh>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Cosh::normalize() const {
    return std::make_shared<Cosh>(arg_->normalize());
}

int Tanh::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Tanh>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Tanh::normalize() const {
    return std::make_shared<Tanh>(arg_->normalize());
}

int Sech::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sech>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Sech::normalize() const {
    return std::make_shared<Sech>(arg_->normalize());
}

int Csch::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Csch>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Csch::normalize() const {
    return std::make_shared<Csch>(arg_->normalize());
}

int Coth::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Coth>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Coth::normalize() const {
    return std::make_shared<Coth>(arg_->normalize());
}

int Arcsinh::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arcsinh>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arcsinh::normalize() const {
    return std::make_shared<Arcsinh>(arg_->normalize());
}

int Arccosh::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arccosh>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arccosh::normalize() const {
    return std::make_shared<Arccosh>(arg_->normalize());
}

int Arctanh::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arctanh>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arctanh::normalize() const {
    return std::make_shared<Arctanh>(arg_->normalize());
}

int Arcsech::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arcsech>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arcsech::normalize() const {
    return std::make_shared<Arcsech>(arg_->normalize());
}

int Arccsch::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arccsch>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arccsch::normalize() const {
    return std::make_shared<Arccsch>(arg_->normalize());
}

int Arccoth::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Arccoth>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Arccoth::normalize() const {
    return std::make_shared<Arccoth>(arg_->normalize());
}

int Abs::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Abs>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Abs::normalize() const {
    return std::make_shared<Abs>(arg_->normalize());
}

int Sign::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sign>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Sign::normalize() const {
    return std::make_shared<Sign>(arg_->normalize());
}

int Sqrt::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sqrt>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Sqrt::normalize() const {
    return std::make_shared<Sqrt>(arg_->normalize());
}

int Root::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Root>(other);
    int cmp_index = index_->compare(o->index_);
    if(cmp_index != 0) return cmp_index;
    return radicand_->compare(o->radicand_);
} 
ExprPtr Root::normalize() const {
    return std::make_shared<Root>(index_->normalize(), radicand_->normalize());
}

int Log::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Log>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Log::normalize() const {
    return std::make_shared<Log>(arg_->normalize());
}

int Exp::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Exp>(other);
    return arg_->compare(o->arg_);
} 
ExprPtr Exp::normalize() const {
    return std::make_shared<Exp>(arg_->normalize());
}

int LogBase::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<LogBase>(other);
    int cmp_base = base_->compare(o->base_);
    if(cmp_base != 0) return cmp_base;
    return arg_->compare(o->arg_);
} 
ExprPtr LogBase::normalize() const {
    return std::make_shared<LogBase>(base_->normalize(), arg_->normalize());
}

int Pow::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Pow>(other);
    int cmp_left = left_->compare(o->left_);
    if(cmp_left != 0) return cmp_left;
    return right_->compare(o->right_);
} 
ExprPtr Pow::normalize() const {
    return std::make_shared<Pow>(left_->normalize(), right_->normalize());
}

int Sub::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Sub>(other);
    int cmp_left = left_->compare(o->left_);
    if(cmp_left != 0) return cmp_left;
    return right_->compare(o->right_);
} 
ExprPtr Sub::normalize() const {
    return std::make_shared<Sub>(left_->normalize(), right_->normalize());
}

int Div::compare_same_type(const ExprPtr&other) const {
    auto mul = left_->mul(right_->pow(m_one));
    return mul->compare_same_type(other);
} 
ExprPtr Div::normalize() const {
    return std::make_shared<Mul>(left_->normalize(), (right_->pow(m_one))->normalize());
}

int Add::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Add>(other);
    int cmp_left = left_->compare(o->left_);
    if(cmp_left != 0) return cmp_left;
    return right_->compare(o->right_);
} 
ExprPtr Add::normalize() const {
    std::vector<ExprPtr> terms;
    flatten_add(std::const_pointer_cast<Expr>(shared_from_this()), terms);
    std::sort(terms.begin(), terms.end(), Expr::less);
    ExprPtr result = terms.back();
    for(size_t i = terms.size() - 1; i > 0; -- i){
        result = std::make_shared<Add>(terms[i - 1], result);
    }
    return result;
}

int Mul::compare_same_type(const ExprPtr&other) const {
    auto o = std::dynamic_pointer_cast<Mul>(other);
    int cmp_left = left_->compare(o->left_);
    if(cmp_left != 0) return cmp_left;
    return right_->compare(o->right_);
} 

ExprPtr Mul::normalize() const {
    std::vector<ExprPtr> factors;
    flatten_mul(std::const_pointer_cast<Expr>(shared_from_this()), factors);
    std::sort(factors.begin(), factors.end(), Expr::less);
    if(factors.empty()) return std::make_shared<Number>(1.0);
    ExprPtr result = factors.back();
    for(size_t i = factors.size() - 1; i > 0; -- i){
        result = std::make_shared<Mul>(factors[i - 1], result);
    }
    return result;
}

// ---- Derivadas ----
// -- Trigonometricas y exponenciales y logaritmicas

// sin(u)  →  u' * cos(u)
ExprPtr Sin::derivative(const std::string& var) const {
    return std::make_shared<Cos>(arg_)->mul(arg_->derivative(var));
}

// cos(u)  →  - u' * sin(u)
ExprPtr Cos::derivative(const std::string& var) const {
    auto neg = std::make_shared<Number>(-1.0);
    return neg->mul(std::make_shared<Sin>(arg_))->mul(arg_->derivative(var));
}

// tan(u)  →  u' * sec(u) ^ 2
ExprPtr Tan::derivative(const std::string& var) const {
    auto two = std::make_shared<Number>(2.0);
    return std::make_shared<Pow>(std::make_shared<Sec>(arg_), two)->mul(arg_->derivative(var));
}

// sec(u)  →  u' * sec(u) * tan(u)
ExprPtr Sec::derivative(const std::string& var) const {
    return (std::make_shared<Sec>(arg_)->mul(std::make_shared<Tan>(arg_)))->mul(arg_->derivative(var));
}

// csc(u)  →  - u' * csc(u) * cot(u) 
ExprPtr Csc::derivative(const std::string& var) const {
    auto neg = std::make_shared<Number>(-1.0);
    return neg->mul(std::make_shared<Csc>(arg_))->mul(std::make_shared<Cot>(arg_))->mul(arg_->derivative(var));
}

// cot(u)  →  - u' * csc(u)^2 
ExprPtr Cot::derivative(const std::string& var) const {
    auto two = std::make_shared<Number>(2.0);
    auto neg = std::make_shared<Number>(-1.0);
    return neg->mul(std::make_shared<Pow>(std::make_shared<Csc>(arg_), two))->mul(arg_->derivative(var));
}

// arcsin(u)  →  u' / sqrt(1 - u^2)
ExprPtr Arcsin::derivative(const std::string& var) const {
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto half = std::make_shared<Number>(0.5);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto denom = std::make_shared<Pow>(std::make_shared<Sub>(one, u_sq), half);
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// arccos(u)  →  -u' / sqrt(1 - u^2)
ExprPtr Arccos::derivative(const std::string& var) const {
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto half = std::make_shared<Number>(0.5);
    auto neg = std::make_shared<Number>(-1.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto denom = std::make_shared<Pow>(std::make_shared<Sub>(one, u_sq), half);
    return neg->mul(arg_->derivative(var))->mul(denom->pow(make_number(-1.0)));
}

// arctan(u)  →  u' / (1 + u^2)
ExprPtr Arctan::derivative(const std::string& var) const {
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto denom = std::make_shared<Add>(one, u_sq);
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// arcsec(u)  →  u' / (|u| * sqrt(u^2 - 1))
ExprPtr Arcsec::derivative(const std::string& var) const {
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto half = std::make_shared<Number>(0.5);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto radic = std::make_shared<Sub>(u_sq, one);
    auto sqrt_part = std::make_shared<Pow>(radic, half);
    auto denom = std::make_shared<Mul>(
        std::make_shared<Abs>(arg_),
        sqrt_part
    );
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// arccsc(u)  →  -u' / (|u| * sqrt(u^2 - 1))
ExprPtr Arccsc::derivative(const std::string& var) const {
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto half = std::make_shared<Number>(0.5);
    auto neg = std::make_shared<Number>(-1.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto radic = std::make_shared<Sub>(u_sq, one);
    auto sqrt_part = std::make_shared<Pow>(radic, half);
    auto denom = std::make_shared<Mul>(
        std::make_shared<Abs>(arg_),
        sqrt_part
    );
    return neg->mul(arg_->derivative(var))->mul(denom->pow(make_number(-1.0)));
}

// arccot(u)  →  -u' / (1 + u^2)
ExprPtr Arccot::derivative(const std::string& var) const {
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto neg = std::make_shared<Number>(-1.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto denom = std::make_shared<Add>(one, u_sq);
    return neg->mul(arg_->derivative(var))->mul(denom->pow(make_number(-1.0)));
}

// Sinh
ExprPtr Sinh::derivative(const std::string& var) const {
    return std::make_shared<Cosh>(arg_)->mul(arg_->derivative(var));
}

// Cosh
ExprPtr Cosh::derivative(const std::string& var) const {
    return std::make_shared<Sinh>(arg_)->mul(arg_->derivative(var));
}

// Tanh
ExprPtr Tanh::derivative(const std::string& var) const {
    // d/dx tanh(u) = sech²(u) * u'
    auto two = std::make_shared<Number>(2.0);
    return std::make_shared<Pow>(std::make_shared<Sech>(arg_), two)->mul(arg_->derivative(var));
}

// Coth
ExprPtr Coth::derivative(const std::string& var) const {
    // d/dx coth(u) = -csch²(u) * u'
    auto two = std::make_shared<Number>(2.0);
    auto neg = std::make_shared<Number>(-1.0);
    return neg->mul(std::make_shared<Pow>(std::make_shared<Csch>(arg_), two))->mul(arg_->derivative(var));
}

// Sech
ExprPtr Sech::derivative(const std::string& var) const {
    // d/dx sech(u) = -sech(u)*tanh(u) * u'
    auto neg = std::make_shared<Number>(-1.0);
    auto term = std::make_shared<Mul>(std::make_shared<Sech>(arg_), std::make_shared<Tanh>(arg_));
    return neg->mul(term)->mul(arg_->derivative(var));
}

// Csch
ExprPtr Csch::derivative(const std::string& var) const {
    // d/dx csch(u) = -csch(u)*coth(u) * u'
    auto neg = std::make_shared<Number>(-1.0);
    auto term = std::make_shared<Mul>(std::make_shared<Csch>(arg_), std::make_shared<Coth>(arg_));
    return neg->mul(term)->mul(arg_->derivative(var));
}

// Arcsinh
ExprPtr Arcsinh::derivative(const std::string& var) const {
    // 1 / sqrt(u^2 + 1)
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto radic = std::make_shared<Add>(u_sq, one);
    auto denom = std::make_shared<Sqrt>(radic);
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// Arccosh
ExprPtr Arccosh::derivative(const std::string& var) const {
    // 1 / sqrt(u^2 - 1)
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto radic = std::make_shared<Sub>(u_sq, one);
    auto denom = std::make_shared<Sqrt>(radic);
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// Arctanh
ExprPtr Arctanh::derivative(const std::string& var) const {
    // 1 / (1 - u^2)
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto denom = std::make_shared<Sub>(one, u_sq);
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// Arccoth (misma fórmula, dominio diferente)
ExprPtr Arccoth::derivative(const std::string& var) const {
    // 1 / (1 - u^2)
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto denom = std::make_shared<Sub>(one, u_sq);
    return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
}

// Arcsech
ExprPtr Arcsech::derivative(const std::string& var) const {
    // -1 / (u * sqrt(1 - u^2))
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto neg = std::make_shared<Number>(-1.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto radic = std::make_shared<Sub>(one, u_sq);
    auto sqrt_part = std::make_shared<Sqrt>(radic);
    auto denom = std::make_shared<Mul>(arg_, sqrt_part);
    return neg->mul(arg_->derivative(var))->mul(denom->pow(make_number(-1.0)));
}

// Arccsch
ExprPtr Arccsch::derivative(const std::string& var) const {
    // -1 / (|u| * sqrt(1 + u^2))
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto neg = std::make_shared<Number>(-1.0);
    auto u_sq = std::make_shared<Pow>(arg_, two);
    auto radic = std::make_shared<Add>(u_sq, one);
    auto sqrt_part = std::make_shared<Sqrt>(radic);
    auto abs_u = std::make_shared<Abs>(arg_);
    auto denom = std::make_shared<Mul>(abs_u, sqrt_part);
    return neg->mul(arg_->derivative(var))->mul(denom->pow(make_number(-1.0)));
}

// log(u)  →  u' / u
ExprPtr Log::derivative(const std::string& var) const {
    return arg_->derivative(var)->mul(arg_->pow(make_number(-1.0)));
}

// exp(u)  →  exp(u) * u'
ExprPtr Exp::derivative(const std::string& var) const {
    return std::make_shared<Exp>(arg_)->mul(arg_->derivative(var));
}
ExprPtr Root::derivative(const std::string& var) const {
    // d/dx (u^(1/n)) = (1/n) * u^(1/n - 1) * u'
    auto one = std::make_shared<Number>(1.0);
    auto inv_index = one->mul(index_->pow(make_number(-1.0)));
    auto new_exp = inv_index->add(m_one);
    auto pow_part = std::make_shared<Pow>(radicand_, new_exp);
    return inv_index->mul(pow_part)->mul(radicand_->derivative(var));
}

// -- Aritmeticas 
ExprPtr Add::derivative(const std::string& var) const {
    return left_->derivative(var)->add(right_->derivative(var));
}
ExprPtr Mul::derivative(const std::string& var) const {
    auto du = left_->derivative(var);
    auto dv = right_->derivative(var);
    return du->mul(right_)->add(left_->mul(dv));
}
ExprPtr Sub::derivative(const std::string& var) const {
    return left_->derivative(var)->add(right_->derivative(var)->mul(m_one));
}
ExprPtr Div::derivative(const std::string& var) const {
    auto du = left_->derivative(var);
    auto dv = right_->derivative(var);
    auto numerator = du->mul(right_)->add(left_->mul(dv)->mul(m_one));
    auto denominator = right_->mul(right_);
    return numerator->mul(denominator->pow(make_number(-1.0)));
}
ExprPtr Pow::derivative(const std::string& var) const {
    auto exp_num = std::dynamic_pointer_cast<Number>(right_);
    if (exp_num) {
        double n = exp_num->value();
        auto new_exp = std::make_shared<Number>(n - 1.0);
        auto base_deriv = left_->derivative(var);
        auto factor = std::make_shared<Number>(n);
        auto power_term = left_->pow(new_exp);
        return factor->mul(power_term)->mul(base_deriv);
    } else {
        return std::const_pointer_cast<Expr>(shared_from_this())->mul(
            std::make_shared<Add>(
                right_->derivative(var)->mul(std::make_shared<Log>(left_)),
                right_->mul(
                    std::make_shared<Mul>(
                        left_->derivative(var),
                        left_->pow(m_one)
                    )
                )
            )
        );
    }
}
// d/dx |u| = sign(u) * u'
ExprPtr Abs::derivative(const std::string& var) const {
    return std::make_shared<Sign>(arg_)->mul(arg_->derivative(var));
}
ExprPtr Sign::derivative(const std::string& var) const {
    return std::make_shared<Number>(0.0);
}


// ---- Implementación de Simplify para todas las clases ----
ExprPtr Add::simplify() const {
    auto left_s = left_->simplify();
    auto right_s = right_->simplify();

    // 1. Aplanar todos los términos
    std::vector<ExprPtr> terms;
    flatten_add(left_s, terms);
    flatten_add(right_s, terms);

    // 2. Extraer coeficientes y normalizar las bases
    struct Term { double coef; ExprPtr base; };
    std::vector<Term> extracted;
    for (const auto& t : terms) {
        auto [coef, base] = extract_coeff(t);
        // normalize() asegura que (x*y) y (y*x) sean idénticos
        extracted.push_back({coef, base->normalize()}); 
    }

    // 3. Ordenar términos usando tu método compare
    std::sort(extracted.begin(), extracted.end(), [](const Term& a, const Term& b) {
        return a.base->compare(b.base) < 0; 
    });

    // 4. Combinar términos semejantes (ahora están contiguos gracias al sort)
    std::vector<Term> combined;
    for (const auto& term : extracted) {
        if (!combined.empty() && combined.back().base->is_equal(term.base)) {
            combined.back().coef += term.coef; // Sumar coeficientes (Ej: 2x + 3x = 5x)
        } else {
            combined.push_back(term);
        }
    }

    // 5. Reconstruir la expresión final
    std::vector<ExprPtr> final_terms;
    for (const auto& term : combined) {
        if (term.coef == 0.0) continue; // Eliminar ceros (Ej: x - x = 0x -> fuera)
        
        // Si la base es "1", es solo un número
        if (auto num_base = std::dynamic_pointer_cast<Number>(term.base)) {
            if (num_base->value() == 1.0) {
                final_terms.push_back(std::make_shared<Number>(term.coef));
                continue;
            }
        }
        
        if (term.coef == 1.0) {
            final_terms.push_back(term.base);
        } else {
            final_terms.push_back(std::make_shared<Mul>(std::make_shared<Number>(term.coef), term.base));
        }
    }

    if (final_terms.empty()) return std::make_shared<Number>(0.0);
    if (final_terms.size() == 1) return final_terms[0];

    // Reensamblar el árbol Add
    ExprPtr result = final_terms[0];
    for (size_t i = 1; i < final_terms.size(); ++i) {
        double val;
        if(is_number(final_terms[i], val)){ 
            if(val == 0.0) continue;
        } else 
        result = std::make_shared<Add>(result, final_terms[i]);
    }
    
    return result;
}

ExprPtr Mul::simplify() const {
    auto left_s = left_->simplify();
    auto right_s = right_->simplify();

    // 1. Aplanar todos los factores usando tu función
    std::vector<ExprPtr> factors;
    flatten_mul(left_s, factors);
    flatten_mul(right_s, factors);

    double numeric_coeff = 1.0;
    struct Factor { ExprPtr base; double exp; };
    std::vector<Factor> extracted;

    // 2. Extraer bases/exponentes y agrupar números puros
    for (const auto& f : factors) {
        // Si el factor es un número puro, lo multiplicamos al coeficiente global
        if (auto num = std::dynamic_pointer_cast<Number>(f)) {
            numeric_coeff *= num->value();
            continue;
        }
        
        auto [base, exp] = extract_exponent(f);
        // normalize() previene que (x+y) y (y+x) se vean como bases distintas
        extracted.push_back({base->normalize(), exp});
    }

    // Si todo se multiplica por 0, el resultado entero colapsa a 0
    if (numeric_coeff == 0.0) {
        return std::make_shared<Number>(0.0);
    }

    // 3. Ordenar las bases para que los términos idénticos queden juntos
    std::sort(extracted.begin(), extracted.end(), [](const Factor& a, const Factor& b) {
        return a.base->compare(b.base) < 0;
    });

    // 4. Combinar bases iguales sumando sus exponentes
    std::vector<Factor> combined;
    for (const auto& factor : extracted) {
        if (!combined.empty() && combined.back().base->is_equal(factor.base)) {
            combined.back().exp += factor.exp; // x^2 * x^3 = x^5
        } else {
            combined.push_back(factor);
        }
    }

    // 5. Reconstruir la expresión final
    std::vector<ExprPtr> final_factors;
    
    // Añadimos el coeficiente numérico global (si no es 1, o si la expresión está vacía)
    if (numeric_coeff != 1.0 || combined.empty()) {
        final_factors.push_back(std::make_shared<Number>(numeric_coeff));
    }

    for (const auto& factor : combined) {
        if (factor.exp == 0.0) continue; // x^0 = 1, se descarta del producto
        
        if (factor.exp == 1.0) {
            final_factors.push_back(factor.base);
        } else {
            final_factors.push_back(std::make_shared<Pow>(factor.base, std::make_shared<Number>(factor.exp)));
        }
    }

    // Casos borde de reconstrucción
    if (final_factors.empty()) return std::make_shared<Number>(1.0);
    if (final_factors.size() == 1) return final_factors[0];

    // Reensamblar el árbol Mul
    ExprPtr result = final_factors[0];
    for (size_t i = 1; i < final_factors.size(); ++i) {
        result = std::make_shared<Mul>(result, final_factors[i]);
    }

    return result;
}

ExprPtr Sub::simplify() const {
    auto left_s = left_->simplify();
    auto right_s = right_->simplify();

    if (auto zero = std::dynamic_pointer_cast<Number>(right_s)) {
        if (zero->value() == 0.0) return left_s;
    }

    auto lnum = std::dynamic_pointer_cast<Number>(left_s);
    auto rnum = std::dynamic_pointer_cast<Number>(right_s);
    if (lnum && rnum) {
        return std::make_shared<Number>(lnum->value() - rnum->value());
    }

    if (left_s->is_equal(right_s)) {
        return std::make_shared<Number>(0.0);
    }


    auto neg_one = std::make_shared<Number>(-1.0);
    auto neg_r = std::make_shared<Mul>(neg_one, right_s)->simplify();
    
    return std::make_shared<Add>(left_s, neg_r)->simplify();
}

ExprPtr Div::simplify() const {
    auto left_s = left_->simplify();
    auto right_s = right_->simplify();

    if (auto one = std::dynamic_pointer_cast<Number>(right_s)) {
        if (one->value() == 1.0) return left_s;
    }

    if (left_s->is_equal(right_s)) {
        return std::make_shared<Number>(1.0);
    }

    if (auto zero = std::dynamic_pointer_cast<Number>(left_s)) {
        if (zero->value() == 0.0) return std::make_shared<Number>(0.0);
    }

    auto lnum = std::dynamic_pointer_cast<Number>(left_s);
    auto rnum = std::dynamic_pointer_cast<Number>(right_s);
    if (lnum && rnum && rnum->value() != 0.0) {
        return std::make_shared<Number>(lnum->value() / rnum->value());
    }

    auto neg_one = std::make_shared<Number>(-1.0);
    auto inv_r = std::make_shared<Pow>(right_s, neg_one)->simplify();
    
    return std::make_shared<Mul>(left_s, inv_r)->simplify();
}

ExprPtr Pow::simplify() const {
    auto base_s = left_->simplify();
    auto exp_s = right_->simplify();

    if (auto n_exp = std::dynamic_pointer_cast<Number>(exp_s)) {
        if (n_exp->value() == 0.0) return std::make_shared<Number>(1.0);
        if (n_exp->value() == 1.0) return base_s;
    }

    if (auto zero = std::dynamic_pointer_cast<Number>(base_s)) {
        if (zero->value() == 0.0) {
            if (auto exp_num = std::dynamic_pointer_cast<Number>(exp_s)) {
                if (exp_num->value() > 0.0) return std::make_shared<Number>(0.0);
            }
        }
    }

    double base_val, exp_val;
    if(is_number(base_s, base_val) && is_number(exp_s, exp_val)){
        if(base_val < 0 && std::floor(exp_val) != exp_val){
            auto pos_base_pow = make_number(std::pow(-base_val, exp_val));
            auto theta = std::make_shared<Mul>(make_pi(), make_number(exp_val));

            auto real_part = std::make_shared<Mul>(pos_base_pow ,std::make_shared<Cos>(theta));
            auto imag_part = std::make_shared<Mul>(pos_base_pow ,std::make_shared<Sin>(theta));

            return std::make_shared<Add>(real_part, std::make_shared<Mul>(imag_part, make_i()))->simplify();
        }
    }

    if (auto one = std::dynamic_pointer_cast<Number>(base_s)) {
        if (one->value() == 1.0) return std::make_shared<Number>(1.0);
    }

    // (x^a)^b = x^(a*b)
    if (auto inner_pow = std::dynamic_pointer_cast<Pow>(base_s)) {
        auto inner_exp = std::dynamic_pointer_cast<Number>(inner_pow->right_);
        auto outer_exp = std::dynamic_pointer_cast<Number>(exp_s);
        if (inner_exp && outer_exp) {
            double new_exp = inner_exp->value() * outer_exp->value();
            return std::make_shared<Pow>(inner_pow->left_, std::make_shared<Number>(new_exp));
        }
    }

    // Sqrt(x) ** 2 = x
    if(auto n_exp = std::dynamic_pointer_cast<Number>(exp_s)){
        if (n_exp && n_exp->value() == 2.0)
        {
            if (auto base_sqrt = std::dynamic_pointer_cast<Sqrt>(base_s)){
                return base_sqrt->arg_->simplify();
            }
        }
        
    }

    auto bnum = std::dynamic_pointer_cast<Number>(base_s);
    auto enum_ = std::dynamic_pointer_cast<Number>(exp_s);
    if (bnum && enum_) {
        return std::make_shared<Number>(std::pow(bnum->value(), enum_->value()));
    }

    return std::make_shared<Pow>(base_s, exp_s);
}

// Sin::simplify
ExprPtr Sin::simplify() const {
    auto s_arg = arg_->simplify();
    
    // 1. Valores exactos
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num && num->value() == 0.0) return std::make_shared<Number>(0.0);
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    if (cst && cst->to_string() == "pi") return std::make_shared<Number>(0.0);
    else if (num) return std::make_shared<Number>(std::sin(num->value()));
    
    // 2. Inversa: sin(arcsin(x)) = x
    if (auto arcsin = std::dynamic_pointer_cast<Arcsin>(s_arg)) return arcsin->arg_;
    
    // 3. Paridad (Impar): sin(-x) = -sin(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Sin>(mul->right_))->simplify();
            }
        }
    }

    return std::make_shared<Sin>(s_arg);
}

// Cos::simplify
ExprPtr Cos::simplify() const {
    auto s_arg = arg_->simplify();
    
    // 1. Valores exactos
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num && num->value() == 0.0) return std::make_shared<Number>(1.0);
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    if (cst && cst->to_string() == "pi") return std::make_shared<Number>(-1.0);
    else if (num) return std::make_shared<Number>(std::cos(num->value()));

    // 2. Inversa: cos(arccos(x)) = x
    if (auto arccos = std::dynamic_pointer_cast<Arccos>(s_arg)) return arccos->arg_;

    // 3. Paridad (Par): cos(-x) = cos(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Cos>(mul->right_)->simplify();
            }
        }
    }

    return std::make_shared<Cos>(s_arg);
}

// Tan::simplify
ExprPtr Tan::simplify() const {
    auto s_arg = arg_->simplify();
    
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num && num->value() == 0.0) return std::make_shared<Number>(0.0);
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    if (cst && cst->to_string() == "pi") return std::make_shared<Number>(0.0);
    else if (num) return std::make_shared<Number>(std::tan(num->value()));

    // Inversa: tan(arctan(x)) = x
    if (auto arctan = std::dynamic_pointer_cast<Arctan>(s_arg)) return arctan->arg_;

    // Paridad (Impar): tan(-x) = -tan(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Tan>(mul->right_))->simplify();
            }
        }
    }

    return std::make_shared<Tan>(s_arg);
}

// Sec::simplify
ExprPtr Sec::simplify() const {
    auto s_arg = arg_->simplify();
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    if (cst && cst->to_string() == "pi") return std::make_shared<Number>(-1.0); // Nota: el tuyo devolvía 0.0, sec(pi) es -1
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num && num->value() == 0.0) return std::make_shared<Number>(1.0); // sec(0) = 1
    if (num) return std::make_shared<Number>(1 / std::cos(num->value()));

    // Inversa: sec(arcsec(x)) = x
    if (auto arcsec = std::dynamic_pointer_cast<Arcsec>(s_arg)) return arcsec->arg_;

    // Paridad (Par): sec(-x) = sec(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Sec>(mul->right_)->simplify();
            }
        }
    }

    return std::make_shared<Sec>(s_arg);
}

// Csc::simplify
ExprPtr Csc::simplify() const {
    auto s_arg = arg_->simplify();
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    // csc(pi) -> infinito o no definido. Si tu CAS usa Infinite, retórnalo aquí.
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) return std::make_shared<Number>(1 / std::sin(num->value()));

    // Inversa: csc(arccsc(x)) = x
    if (auto arccsc = std::dynamic_pointer_cast<Arccsc>(s_arg)) return arccsc->arg_;

    // Paridad (Impar): csc(-x) = -csc(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Csc>(mul->right_))->simplify();
            }
        }
    }

    return std::make_shared<Csc>(s_arg);
}

// Cot::simplify
ExprPtr Cot::simplify() const {
    auto s_arg = arg_->simplify();
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    // cot(pi) -> infinito o no definido.
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) return std::make_shared<Number>(1 / std::tan(num->value()));

    // Inversa: cot(arccot(x)) = x
    if (auto arccot = std::dynamic_pointer_cast<Arccot>(s_arg)) return arccot->arg_;

    // Paridad (Impar): cot(-x) = -cot(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Cot>(mul->right_))->simplify();
            }
        }
    }

    return std::make_shared<Cot>(s_arg);
}

ExprPtr Arcsin::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        double v = num->value();
        if (v == 0.0) return std::make_shared<Number>(0.0);
        if (v == 1.0) return make_pi()->mul(make_number(0.5));
        if (v == -1.0) return make_pi()->mul(make_number(-0.5));
        return std::make_shared<Number>(std::asin(v));
    }
    // Inversa directa
    if (auto sin_expr = std::dynamic_pointer_cast<Sin>(s_arg)) return sin_expr->arg_;
    // Paridad (Impar): arcsin(-x) = -arcsin(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arcsin>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arcsin>(s_arg);
}

ExprPtr Arccos::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        double v = num->value();
        if (v == 0.0) return make_pi()->mul(make_number(0.5));
        if (v == 1.0) return std::make_shared<Number>(0.0);
        if (v == -1.0) return make_pi();
        return std::make_shared<Number>(std::acos(v));
    }
    // Inversa directa
    if (auto cos_expr = std::dynamic_pointer_cast<Cos>(s_arg)) return cos_expr->arg_;
    // arccos(-x) = pi - arccos(x) (opcional, lo dejo simbólico por defecto)
    return std::make_shared<Arccos>(s_arg);
}

ExprPtr Arctan::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        double v = num->value();
        if (v == 0.0) return std::make_shared<Number>(0.0);
        if (v == 1.0) return make_pi()->mul(make_number(0.25));
        if (v == -1.0) return make_pi()->mul(make_number(-0.25));
        return std::make_shared<Number>(std::atan(v));
    }
    if (std::dynamic_pointer_cast<Infinite>(s_arg)) return make_pi()->mul(make_number(0.5));
    
    // Inversa directa
    if (auto tan_expr = std::dynamic_pointer_cast<Tan>(s_arg)) return tan_expr->arg_;
    
    // Paridad (Impar)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arctan>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arctan>(s_arg);
}

// Aplicar lógica idéntica de inversa directa para Arcsec, Arccsc y Arccot...
ExprPtr Arcsec::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        double v = num->value();
        if (v == 1.0) return std::make_shared<Number>(0.0);
        if (v == -1.0) return make_pi();
        if (std::abs(v) >= 1.0) return std::make_shared<Number>(std::acos(1.0 / v));
    }
    if (auto sec_expr = std::dynamic_pointer_cast<Sec>(s_arg)) return sec_expr->arg_;
    return std::make_shared<Arcsec>(s_arg);
}

ExprPtr Arccsc::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        double v = num->value();
        if (v == 1.0) return make_pi()->mul(make_number(0.5));
        if (v == -1.0) return make_pi()->mul(make_number(-0.5));
        if (std::abs(v) >= 1.0) return std::make_shared<Number>(std::asin(1.0 / v));
    }
    if (auto csc_expr = std::dynamic_pointer_cast<Csc>(s_arg)) return csc_expr->arg_;
    
    // Paridad (Impar)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arccsc>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arccsc>(s_arg);
}

ExprPtr Arccot::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        double v = num->value();
        if (v == 0.0) return make_pi()->mul(make_number(0.5));
        if (v == 1.0) return make_pi()->mul(make_number(0.25));
        return std::make_shared<Number>(std::atan(1.0 / v));
    }
    if (std::dynamic_pointer_cast<Infinite>(s_arg)) return std::make_shared<Number>(0.0);
    if (auto cot_expr = std::dynamic_pointer_cast<Cot>(s_arg)) return cot_expr->arg_;
    return std::make_shared<Arccot>(s_arg);
}

// Sinh::simplify
ExprPtr Sinh::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        return std::make_shared<Number>(std::sinh(num->value()));
    }
    if (auto zero = std::dynamic_pointer_cast<Number>(s); zero && zero->value() == 0.0)
        return std::make_shared<Number>(0.0);
    
    // Inversa: sinh(arcsinh(x)) = x
    if (auto arcsinh = std::dynamic_pointer_cast<Arcsinh>(s)) return arcsinh->arg_;

    // Paridad (Impar): sinh(-x) = -sinh(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Sinh>(mul->right_))->simplify();
            }
        }
    }
    return std::make_shared<Sinh>(s);
}

// Cosh::simplify
ExprPtr Cosh::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        return std::make_shared<Number>(std::cosh(num->value()));
    }
    if (auto zero = std::dynamic_pointer_cast<Number>(s); zero && zero->value() == 0.0)
        return std::make_shared<Number>(1.0);
    
    // Inversa: cosh(arccosh(x)) = x
    if (auto arccosh = std::dynamic_pointer_cast<Arccosh>(s)) return arccosh->arg_;

    // Paridad (Par): cosh(-x) = cosh(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Cosh>(mul->right_)->simplify();
            }
        }
    }
    return std::make_shared<Cosh>(s);
}

// Tanh::simplify
ExprPtr Tanh::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        return std::make_shared<Number>(std::tanh(num->value()));
    }
    if (auto zero = std::dynamic_pointer_cast<Number>(s); zero && zero->value() == 0.0)
        return std::make_shared<Number>(0.0);
    
    // Inversa: tanh(arctanh(x)) = x
    if (auto arctanh = std::dynamic_pointer_cast<Arctanh>(s)) return arctanh->arg_;

    // Paridad (Impar): tanh(-x) = -tanh(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Tanh>(mul->right_))->simplify();
            }
        }
    }
    return std::make_shared<Tanh>(s);
}

// Coth::simplify
ExprPtr Coth::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v == 0.0) return make_inf(); // coth(0) -> Infinito
        return std::make_shared<Number>(1.0 / std::tanh(v));
    }
    
    // Inversa: coth(arccoth(x)) = x
    if (auto arccoth = std::dynamic_pointer_cast<Arccoth>(s)) return arccoth->arg_;

    // Paridad (Impar): coth(-x) = -coth(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Coth>(mul->right_))->simplify();
            }
        }
    }
    return std::make_shared<Coth>(s);
}

// Sech::simplify
ExprPtr Sech::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        return std::make_shared<Number>(1.0 / std::cosh(num->value()));
    }
    if (auto zero = std::dynamic_pointer_cast<Number>(s); zero && zero->value() == 0.0)
        return std::make_shared<Number>(1.0);
    
    // Inversa: sech(arcsech(x)) = x
    if (auto arcsech = std::dynamic_pointer_cast<Arcsech>(s)) return arcsech->arg_;

    // Paridad (Par): sech(-x) = sech(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Sech>(mul->right_)->simplify();
            }
        }
    }
    return std::make_shared<Sech>(s);
}

// Csch::simplify
ExprPtr Csch::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v == 0.0) return make_inf(); // csch(0) -> Infinito
        return std::make_shared<Number>(1.0 / std::sinh(v));
    }
    
    // Inversa: csch(arccsch(x)) = x
    if (auto arccsch = std::dynamic_pointer_cast<Arccsch>(s)) return arccsch->arg_;

    // Paridad (Impar): csch(-x) = -csch(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto left_num = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(left_num, std::make_shared<Csch>(mul->right_))->simplify();
            }
        }
    }
    return std::make_shared<Csch>(s);
}

// Arcsinh::simplify
ExprPtr Arcsinh::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        return std::make_shared<Number>(std::asinh(num->value()));
    }
    if (auto zero = std::dynamic_pointer_cast<Number>(s); zero && zero->value() == 0.0)
        return std::make_shared<Number>(0.0);
    
    // Inversa directa: arcsinh(sinh(x)) = x
    if (auto sinh_expr = std::dynamic_pointer_cast<Sinh>(s)) return sinh_expr->arg_;

    // Paridad (Impar): arcsinh(-x) = -arcsinh(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arcsinh>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arcsinh>(s);
}

// Arccosh::simplify
ExprPtr Arccosh::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v < 1.0) return std::make_shared<Arccosh>(s); // Fuera de dominio real
        return std::make_shared<Number>(std::acosh(v));
    }
    if (auto one = std::dynamic_pointer_cast<Number>(s); one && one->value() == 1.0)
        return std::make_shared<Number>(0.0);
    
    // Inversa directa: arccosh(cosh(x)) = x
    if (auto cosh_expr = std::dynamic_pointer_cast<Cosh>(s)) return cosh_expr->arg_;

    return std::make_shared<Arccosh>(s);
}

// Arctanh::simplify
ExprPtr Arctanh::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v <= -1.0 || v >= 1.0) return std::make_shared<Arctanh>(s); // Fuera de dominio
        return std::make_shared<Number>(std::atanh(v));
    }
    if (auto zero = std::dynamic_pointer_cast<Number>(s); zero && zero->value() == 0.0)
        return std::make_shared<Number>(0.0);
    
    // Inversa directa: arctanh(tanh(x)) = x
    if (auto tanh_expr = std::dynamic_pointer_cast<Tanh>(s)) return tanh_expr->arg_;

    // Paridad (Impar): arctanh(-x) = -arctanh(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arctanh>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arctanh>(s);
}

// Arccoth::simplify
ExprPtr Arccoth::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v > -1.0 && v < 1.0) return std::make_shared<Arccoth>(s); // Dominio: |v| > 1
        return std::make_shared<Number>(std::atanh(1.0 / v)); // arccoth(v) = atanh(1/v)
    }
    
    // Inversa directa: arccoth(coth(x)) = x
    if (auto coth_expr = std::dynamic_pointer_cast<Coth>(s)) return coth_expr->arg_;

    // Paridad (Impar): arccoth(-x) = -arccoth(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arccoth>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arccoth>(s);
}

// Arcsech::simplify
ExprPtr Arcsech::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v <= 0.0 || v > 1.0) return std::make_shared<Arcsech>(s); // Dominio (0, 1]
        return std::make_shared<Number>(std::acosh(1.0 / v)); // arcsech(v) = acosh(1/v)
    }
    if (auto one = std::dynamic_pointer_cast<Number>(s); one && one->value() == 1.0)
        return std::make_shared<Number>(0.0);
    
    // Inversa directa: arcsech(sech(x)) = x
    if (auto sech_expr = std::dynamic_pointer_cast<Sech>(s)) return sech_expr->arg_;

    return std::make_shared<Arcsech>(s);
}

// Arccsch::simplify
ExprPtr Arccsch::simplify() const {
    auto s = arg_->simplify();
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v == 0.0) return make_inf();
        return std::make_shared<Number>(std::asinh(1.0 / v)); // arccsch(v) = asinh(1/v)
    }
    
    // Inversa directa: arccsch(csch(x)) = x
    if (auto csch_expr = std::dynamic_pointer_cast<Csch>(s)) return csch_expr->arg_;

    // Paridad (Impar): arccsch(-x) = -arccsch(x)
    if (auto mul = std::dynamic_pointer_cast<Mul>(s)) {
        if (auto ln = std::dynamic_pointer_cast<Number>(mul->left_)) {
            if (ln->value() == -1.0) return std::make_shared<Mul>(ln, std::make_shared<Arccsch>(mul->right_))->simplify();
        }
    }
    return std::make_shared<Arccsch>(s);
}

// Exp::simplify
ExprPtr Exp::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num) {
        if (num->value() == 0.0) return std::make_shared<Number>(1.0);
        if (num->value() == 1.0) return make_e();
    }
    
    // Inversa básica: exp(log(x)) = x
    if (auto log_ptr = std::dynamic_pointer_cast<Log>(s_arg)) {
        return log_ptr->arg_;
    }

    // Regla de oro: exp(n * log(x)) = x^n
    if (auto mul_expr = std::dynamic_pointer_cast<Mul>(s_arg)) {
        if (auto log_right = std::dynamic_pointer_cast<Log>(mul_expr->right_)) {
            return std::make_shared<Pow>(log_right->arg_, mul_expr->left_)->simplify();
        }
        if (auto log_left = std::dynamic_pointer_cast<Log>(mul_expr->left_)) {
            return std::make_shared<Pow>(log_left->arg_, mul_expr->right_)->simplify();
        }
    }
    
    return std::make_shared<Exp>(s_arg);
}

// Log::simplify
ExprPtr Log::simplify() const {
    auto s_arg = arg_->simplify();
    auto num = std::dynamic_pointer_cast<Number>(s_arg);
    if (num && num->value() == 1.0) return std::make_shared<Number>(0.0);
    auto cst = std::dynamic_pointer_cast<Constant>(s_arg);
    if (cst && cst->to_string() == "e") return std::make_shared<Number>(1.0);
    
    double val;
    if(is_number(s_arg, val)){
        if(val < 0){
            auto abs_log = std::make_shared<Log>(make_number(-val))->simplify();
            auto pii = std::make_shared<Mul>(make_pi(), make_i());
            return std::make_shared<Add>(abs_log, pii);
        }
        return make_number(std::log(val));
    }

    // Inversa básica: log(exp(x)) = x
    if (auto exp_ptr = std::dynamic_pointer_cast<Exp>(s_arg)) {
        return exp_ptr->arg_;
    }

    // Regla de potencia: log(x^y) = y * log(x)
    if (auto pow_expr = std::dynamic_pointer_cast<Pow>(s_arg)) {
        return std::make_shared<Mul>(
            pow_expr->right_, 
            std::make_shared<Log>(pow_expr->left_)
        )->simplify();
    }

    return std::make_shared<Log>(s_arg);
}

// Abs::simplify
ExprPtr Abs::simplify() const {
    auto s_arg = arg_->simplify();

    // 1. Evaluar si es número
    if (auto num = std::dynamic_pointer_cast<Number>(s_arg)) {
        return std::make_shared<Number>(std::abs(num->value()));
    }

    // 2. |x| con x ya positivo semánticamente: potencias pares
    if (auto pow_expr = std::dynamic_pointer_cast<Pow>(s_arg)) {
        if (auto exp_num = std::dynamic_pointer_cast<Number>(pow_expr->right_)) {
            double e = exp_num->value();
            // Si el exponente es un entero par, la expresión siempre es ≥0
            if (e == std::floor(e) && std::fmod(e, 2.0) == 0.0) {
                return s_arg;                   
            }
        }
    }

    // 3. | -x | -> |x|
    if (auto mul_expr = std::dynamic_pointer_cast<Mul>(s_arg)) {
        auto left_num = std::dynamic_pointer_cast<Number>(mul_expr->left_);
        if (left_num && left_num->value() == -1.0) {
            return std::make_shared<Abs>(mul_expr->right_)->simplify();
        }
        auto right_num = std::dynamic_pointer_cast<Number>(mul_expr->right_);
        if (right_num && right_num->value() == -1.0) {
            return std::make_shared<Abs>(mul_expr->left_)->simplify();
        }
    }

    // 4. | |x| | -> |x|
    if (std::dynamic_pointer_cast<Abs>(s_arg)) {
        return s_arg;
    }

    return std::make_shared<Abs>(s_arg);
}

// Sign::simplify
ExprPtr Sign::simplify() const {
    auto s_arg = arg_->simplify();

    // 1. Número → -1, 0 o 1
    if (auto num = std::dynamic_pointer_cast<Number>(s_arg)) {
        double v = num->value();
        if (v > 0) return std::make_shared<Number>(1.0);
        if (v < 0) return std::make_shared<Number>(-1.0);
        return std::make_shared<Number>(0.0);
    }

    // 2. sign(-x) = -sign(x)
    if (auto mul_expr = std::dynamic_pointer_cast<Mul>(s_arg)) {
        auto left_num = std::dynamic_pointer_cast<Number>(mul_expr->left_);
        if (left_num) {
            if (left_num->value() == -1.0) {
                return std::make_shared<Mul>(
                    std::make_shared<Number>(-1.0),
                    std::make_shared<Sign>(mul_expr->right_)
                )->simplify();
            }
            // Para factor positivo num >0, sign(num * x) = sign(x) si num>0 (pero cuidado: 0*x)
            if (left_num->value() > 0) {
                return std::make_shared<Sign>(mul_expr->right_)->simplify();
            }
        }
    }

    // 3. sign(|x|) = 1 (si x no es cero, pero simbólicamente asumimos ≠0)
    if (std::dynamic_pointer_cast<Abs>(s_arg)) {
        return std::make_shared<Number>(1.0);
    }

    // 4. sign(sign(x)) = sign(x)
    if (auto inner_sign = std::dynamic_pointer_cast<Sign>(s_arg)) {
        return s_arg;
    }

    return std::make_shared<Sign>(s_arg);
}
ExprPtr Sqrt::simplify() const {
    auto s = arg_->simplify();

    // 1. Evaluación numérica directa
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        double v = num->value();
        if (v < 0) {
            auto positive_root = std::make_shared<Sqrt>(std::make_shared<Number>(-v))->simplify();
            return std::make_shared<Mul>(std::make_shared<ImaginaryUnit>(), positive_root)->simplify();
        }
        return std::make_shared<Number>(std::sqrt(v));
    }

    // 2. sqrt(0) = 0, sqrt(1) = 1
    if (auto num = std::dynamic_pointer_cast<Number>(s)) {
        if (num->value() == 0.0) return std::make_shared<Number>(0.0);
        if (num->value() == 1.0) return std::make_shared<Number>(1.0);
    }

    // 3. sqrt(x^2)  => |x|
    if (auto pow_expr = std::dynamic_pointer_cast<Pow>(s)) {
        if (auto exp_num = std::dynamic_pointer_cast<Number>(pow_expr->right_)) {
            if (exp_num->value() == 2.0) {
                return std::make_shared<Abs>(pow_expr->left_)->simplify();
            }
        }
    }

    // 5. sqrt(exp) -> exp(exp/2)
    if (auto exp_expr = std::dynamic_pointer_cast<Exp>(s)) {
        return std::make_shared<Exp>(
            std::make_shared<Mul>(exp_expr->arg_, std::make_shared<Number>(0.5))
        )->simplify();
    }

    // 6. Si el argumento ya es un Sqrt anidado: sqrt(sqrt(x)) = x^(1/4)
    if (auto inner_sqrt = std::dynamic_pointer_cast<Sqrt>(s)) {
        return std::make_shared<Pow>(
            inner_sqrt->arg_,
            std::make_shared<Number>(0.25)
        )->simplify();
    }

    return std::make_shared<Sqrt>(s);
}

ExprPtr Root::simplify() const {
    auto idx_s = index_->simplify();
    auto rad_s = radicand_->simplify();

    // 1. Si el índice es numérico y el radicando es número: evaluar
    auto idx_num = std::dynamic_pointer_cast<Number>(idx_s);
    auto rad_num = std::dynamic_pointer_cast<Number>(rad_s);
    if (idx_num && rad_num) {
        double n = idx_num->value();
        double x = rad_num->value();
        if (x < 0 && std::fmod(n, 2.0) == 0.0) {
            return std::make_shared<Root>(idx_s, rad_s);
        }
        double result = std::pow(x, 1.0 / n);
        if (x < 0 && std::fmod(n, 2.0) != 0.0) {
            result = -std::pow(-x, 1.0 / n);
        }
        return std::make_shared<Number>(result);
    }

    // 2. root(n, 0) = 0
    if (rad_num && rad_num->value() == 0.0) return std::make_shared<Number>(0.0);
    // 3. root(n, 1) = 1
    if (rad_num && rad_num->value() == 1.0) return std::make_shared<Number>(1.0);

    // 4. root(n, x^n) -> x si n impar, |x| si n par
    if (auto pow_expr = std::dynamic_pointer_cast<Pow>(rad_s)) {
        if (auto exp_num = std::dynamic_pointer_cast<Number>(pow_expr->right_)) {
            if (idx_num) {
                double n = idx_num->value();
                double e = exp_num->value();
                if (e == n) {
                    // x^n bajo raíz n
                    if (std::fmod(n, 2.0) != 0.0 || n == 1.0) {
                        return pow_expr->left_;   // n impar → x
                    } else {
                        return std::make_shared<Abs>(pow_expr->left_)->simplify();
                    }
                }
            }
        }
    }

    // 5. root(m, root(n, x)) = root(m*n, x)
    if (auto inner_root = std::dynamic_pointer_cast<Root>(rad_s)) {
        auto new_index = std::make_shared<Mul>(idx_s, inner_root->index_);
        return std::make_shared<Root>(new_index, inner_root->radicand_)->simplify();
    }

    if (idx_num) {
        return std::make_shared<Pow>(rad_s, std::make_shared<Number>(1.0 / idx_num->value()))->simplify();
    }

    return std::make_shared<Root>(idx_s, rad_s);
}

ExprPtr LogBase::simplify() const {
    auto b = base_->simplify();
    auto x = arg_->simplify();

    // 1. Casos numéricos puros
    auto bnum = std::dynamic_pointer_cast<Number>(b);
    auto xnum = std::dynamic_pointer_cast<Number>(x);
    if (bnum && xnum) {
        double base_v = bnum->value();
        double arg_v = xnum->value();
        if (base_v <= 0.0 || base_v == 1.0 || arg_v <= 0.0) {
            return std::make_shared<LogBase>(b, x);
        }
        return std::make_shared<Number>(std::log(arg_v) / std::log(base_v));
    }

    // 2. log_b(b) = 1
    if (b->is_equal(x)) return std::make_shared<Number>(1.0);

    // 3. log_b(1) = 0
    if (xnum && xnum->value() == 1.0) return std::make_shared<Number>(0.0);

    // 4. log_b(b^x) = x
    if (auto pow_expr = std::dynamic_pointer_cast<Pow>(x)) {
        if (pow_expr->left_->is_equal(b)) {
            return pow_expr->right_;
        }
    }

    // 5. log_b(x^y) = y * log_b(x)
    if (auto pow_expr = std::dynamic_pointer_cast<Pow>(x)) {
        auto exponent = pow_expr->right_;
        auto new_log = std::make_shared<LogBase>(b, pow_expr->left_);
        return std::make_shared<Mul>(exponent, new_log)->simplify();
    }

    // 6. Si la base es e → convertir a Log natural
    if (auto cst = std::dynamic_pointer_cast<Constant>(b)) {
        if (cst->to_string() == "e") {
            return std::make_shared<Log>(x)->simplify();
        }
    }

    // 7. Cambio de base a natural: log_b(x) = log(x)/log(b) (útil si no hay más reglas)
    //    Podrías devolver un Div(Log(x), Log(b)), pero es preferible mantener el nodo.
    return std::make_shared<LogBase>(b, x);
}

// ---- Funciones auxiliares ----
ExprPtr make_number(double v) { return std::make_shared<Number>(v); }
ExprPtr make_variable(const std::string& name) { return std::make_shared<Variable>(name); }
ExprPtr make_pi() { return std::make_shared<Constant>("pi", M_PI); }
ExprPtr make_e() { return std::make_shared<Constant>("e", M_E); }
ExprPtr make_inf() { return std::make_shared<Infinite>(); }
ExprPtr make_i() { return std::make_shared<ImaginaryUnit>(); }


/**
 * Despeje en ecuaciones lineales
 */

ExprPtr Variable::isolate(const std::string& target_var, ExprPtr rhs) const {
    if(target_var == name_){
        return rhs;
    }
    
    return nullptr;
}

ExprPtr Add::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto left_symbols = left_->symbols();

    if(left_symbols.find(target_var) != left_symbols.end()){
        // Esta en el lado izquierdo
        auto new_rhs = std::make_shared<Sub>(rhs, right_)->simplify();
        return left_->isolate(target_var, new_rhs);
    } else {
        // Esta en el derecho
        auto new_rhs = std::make_shared<Sub>(rhs, left_)->simplify();
        return right_->isolate(target_var, new_rhs);
    }
}

ExprPtr Mul::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto left_symbols = left_->symbols();

    if(left_symbols.find(target_var) != left_symbols.end()){
        // Esta en el lado izquierdo
        auto new_rhs = std::make_shared<Div>(rhs, right_)->simplify();
        return left_->isolate(target_var, new_rhs);
    } else {
        // Esta en el derecho
        auto new_rhs = std::make_shared<Div>(rhs, left_)->simplify();
        return right_->isolate(target_var, new_rhs);
    }
}

ExprPtr Div::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto left_symbols = left_->symbols();

    if(left_symbols.find(target_var) != left_symbols.end()){
        // Esta en el lado izquierdo
        auto new_rhs = std::make_shared<Mul>(rhs, right_);
        return left_->isolate(target_var, new_rhs);
    } else {
        // Esta en el derecho
        auto new_rhs = std::make_shared<Div>(left_, rhs)->simplify();
        return right_->isolate(target_var, new_rhs);
    }
}

ExprPtr Sub::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto left_symbols = left_->symbols();

    if(left_symbols.find(target_var) != left_symbols.end()){
        // Esta en el lado izquierdo
        auto new_rhs = std::make_shared<Add>(rhs, right_);
        return left_->isolate(target_var, new_rhs);
    } else {
        // Esta en el derecho
        auto new_rhs = std::make_shared<Sub>(left_, rhs)->simplify();
        return right_->isolate(target_var, new_rhs);
    }
}

ExprPtr Pow::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto base_symbols = left_->symbols();
    auto exp_symbols = right_->symbols();

    bool base_has = base_symbols.find(target_var) != base_symbols.end();
    bool exp_has = exp_symbols.find(target_var) != exp_symbols.end();

    if (base_has && !exp_has) {
        auto one_val = std::make_shared<Number>(1.0);
        auto inv_exp = std::make_shared<Div>(one_val, right_);
        auto new_rhs = std::make_shared<Pow>(rhs, inv_exp);
        return left_->isolate(target_var, new_rhs);
    } 
    else if (!base_has && exp_has) {
        auto new_rhs = std::make_shared<LogBase>(left_, rhs);
        return right_->isolate(target_var, new_rhs);
    }
    
    return nullptr;
}

ExprPtr Abs::isolate(const std::string& target_var, ExprPtr rhs) const {
    return arg_->isolate(target_var, rhs);
}

ExprPtr Sign::isolate(const std::string& target_var, ExprPtr rhs) const {
    return arg_->isolate(target_var, rhs);
}

ExprPtr Exp::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto cleared_rhs = std::make_shared<Log>(rhs);
    return arg_->isolate(target_var, cleared_rhs);
}

ExprPtr Log::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto cleared_rhs = std::make_shared<Exp>(rhs);
    return arg_->isolate(target_var, cleared_rhs);
}

ExprPtr LogBase::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto cleared_rhs = std::make_shared<Pow>(base_, rhs);
    return arg_->isolate(target_var, cleared_rhs);
}

ExprPtr Sqrt::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto cleared_rhs = std::make_shared<Pow>(rhs, two);
    return arg_->isolate(target_var, cleared_rhs);
}

ExprPtr Root::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto cleared_rhs = std::make_shared<Pow>(rhs, index_);
    return radicand_->isolate(target_var, cleared_rhs);
}

ExprPtr Sin::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arcsin>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Cos::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arccos>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Tan::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arctan>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Cot::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arccot>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Sec::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arcsec>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Csc::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arccsc>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arcsin::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Sin>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arccos::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Cos>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arctan::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Tan>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arccot::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Cot>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arcsec::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Sec>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arccsc::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Csc>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Sinh::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arcsinh>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Cosh::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arccosh>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Tanh::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arctanh>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Coth::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arccoth>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Sech::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arcsech>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Csch::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Arccsch>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arcsinh::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Sinh>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arccosh::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Cosh>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arctanh::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Tanh>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arccoth::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Coth>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arcsech::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Sech>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

ExprPtr Arccsch::isolate(const std::string& target_var, ExprPtr rhs) const {
    auto new_rhs = std::make_shared<Csch>(rhs);
    return arg_->isolate(target_var, new_rhs);
}

/**
 * Verificacion de polimonios
 */

bool Variable::is_polynomical(const std::string& target_var) const {
    return true;
}

bool Number::is_polynomical(const std::string& target_var) const {
    return true;
}

bool Constant::is_polynomical(const std::string& target_var) const {
    return true;
}

bool Infinite::is_polynomical(const std::string& target_var) const {
    return true;
}

bool Add::is_polynomical(const std::string& target_var) const {
    return left_->is_polynomical(target_var) && right_->is_polynomical(target_var);
}

bool Sub::is_polynomical(const std::string& target_var) const {
    return left_->is_polynomical(target_var) && right_->is_polynomical(target_var);
}

bool Mul::is_polynomical(const std::string& target_var) const {
    return left_->is_polynomical(target_var) && right_->is_polynomical(target_var);
}

bool Div::is_polynomical(const std::string& target_var) const {
    
    auto denom_symbols = right_->symbols();
    bool denom_has_variable = (denom_symbols.find(target_var) != denom_symbols.end());

    return left_->is_polynomical(target_var) && right_->is_polynomical(target_var) && !denom_has_variable;
}

bool Pow::is_polynomical(const std::string& target_var) const {
    
    auto exp_symbols = right_->symbols();
    bool exp_has_variable = (exp_symbols.find(target_var) != exp_symbols.end());

    if(exp_has_variable){
        return false;
    }

    double exp_val;
    if(is_number(right_, exp_val)){
        return (exp_val >= 0 && std::floor(exp_val) == exp_val && left_->is_polynomical(target_var));
    }

    return false;
}