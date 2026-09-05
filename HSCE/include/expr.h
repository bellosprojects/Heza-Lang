#pragma once
#define EXPR_H
#ifdef EXPR_H

#define _USE_MATH_DEFINES
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <map>
#include <typeinfo>
#include <functional>
#include <variant>
#include <complex>

namespace py = pybind11;

using NumericValue = std::variant<double, std::complex<double>>;

class Expr;
class Number;
class Add;
using ExprPtr = std::shared_ptr<Expr>;
using Substitution = std::unordered_map<std::string, ExprPtr>;

// ---- Clase base ----
class Expr : public std::enable_shared_from_this<Expr> {
public:
    virtual ~Expr() = default;
    virtual std::string to_string() const { return ""; };
    virtual ExprPtr derivative(const std::string& var) const {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    virtual ExprPtr evaluate(const std::map<std::string, double>& vars) const {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    virtual ExprPtr simplify() const {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    virtual bool is_equal(const ExprPtr& other) const;
    virtual int compare(const ExprPtr& other) const;
    virtual int type_id() const {
        return 0;
    };
    virtual int compare_same_type(const ExprPtr& other) const {
        return 0;
    };
    virtual ExprPtr normalize() const {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    static bool less(const ExprPtr& a, const ExprPtr& b);
    virtual int precedence() const { return 4; }
    virtual std::string print(int parent_prec, bool left_side) const {
        return "";
    };
    std::string prints() const {
        return print(0, true);
    }
    virtual void collect_symbols(std::unordered_map<std::string, int>& out) const {};
    virtual std::unordered_map<std::string, int> symbols() const {
        std::unordered_map<std::string, int> s;
        collect_symbols(s);
        return s;
    }
    virtual NumericValue number();
    virtual bool match(const ExprPtr&target, Substitution&env) const {
        return false;
    };
    virtual ExprPtr substitute(const Substitution&env) const {
        return std::const_pointer_cast<Expr>(shared_from_this());
    };
    virtual ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)>& fn) const {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    virtual ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const {
        return nullptr;
    }
    virtual bool is_polynomical(const std::string& target_var) const {
        return false;
    }
    ExprPtr add(const ExprPtr& other);
    ExprPtr add_double(double v);
    ExprPtr mul(const ExprPtr& other);
    ExprPtr mul_double(double v);
    ExprPtr sub(const ExprPtr& other);
    ExprPtr sub_double(double v);
    ExprPtr rsub_double(double v);
    ExprPtr div(const ExprPtr& other);
    ExprPtr div_double(double v);
    ExprPtr rdiv_double(double v);
    ExprPtr pow(const ExprPtr& other);
    ExprPtr pow_double(double v);
    ExprPtr rpow_double(double v);
};

// Comprobar si una expresión es un número con valor específico
bool is_zero(const ExprPtr& e);
bool is_one(const ExprPtr& e);
bool is_minus_one(const ExprPtr& e);
bool is_number(const ExprPtr& e, double& val);
bool is_constant(const ExprPtr& e);
std::complex<double> extract_complex(const ExprPtr& e);

// Extraer coeficiente numérico de un término (para agrupar)
std::pair<double, ExprPtr> extract_coeff(const ExprPtr& e);
std::pair<ExprPtr, double> extract_exponent(const ExprPtr& e);

// Aplanamiento
void flatten_add(const ExprPtr&e, std::vector<ExprPtr>&terms);
void flatten_mul(const ExprPtr&e, std::vector<ExprPtr>&factors);

class RewriteRule {
    private:
        ExprPtr pattern_;
        ExprPtr replacement_;
    public:
        RewriteRule(const ExprPtr& pattern, const ExprPtr& replacement) :
            pattern_(std::move(pattern)), replacement_(std::move(replacement)) {}

        ExprPtr pattern() const { return pattern_; }
        ExprPtr replacement() const { return replacement_; }
        ExprPtr apply(const ExprPtr& expr) const;

};

class RewriteEngine {
    private:
        std::vector<RewriteRule> rules_;

        ExprPtr rewrite_rec(const ExprPtr& expr) const {
            if(!expr) return nullptr;

            ExprPtr rewritten_children = expr->transform_children([this](const ExprPtr &child) {
                return this->rewrite_rec(child);
            });

            ExprPtr current = rewritten_children;
            bool local_changed = true;
            int max_passes = 10;
            int pass = 0;

            while(local_changed && pass < max_passes){
                local_changed = false;
                for(const auto & rule : rules_){
                    if(auto result = rule.apply(current)){
                        current = result;
                        local_changed = true;
                        break;
                    }
                }
                pass++;
            }

            return current;
        }

    public:
        explicit RewriteEngine(std::vector<RewriteRule>& rules) : rules_(std::move(rules)) {}

        void add_rule(const RewriteRule& rule){
            rules_.push_back(rule);
        }

        ExprPtr simplify(const ExprPtr &expr) const {
            ExprPtr current = expr->simplify();
            bool changed = true;
            int iterations = 0;
            const int max_iterations = 50;

            while(changed && iterations < max_iterations){
                ExprPtr normalized = current->simplify();
                ExprPtr rewritten = rewrite_rec(normalized);
                if(rewritten->is_equal(current)){
                    changed = false;
                } else {
                    current = rewritten;
                }
                iterations ++;
            }

            return current;
        }
};

class Wildcard : public Expr {
    private:
        std::string name_;
    public:
        explicit Wildcard(const std::string& name): name_(std::move(name)) {}
        std::string name() const { return name_; }
        std::string to_string() const override {
            return name_;
        }
        std::string print(int, bool) const override {
            return name_;
        }
        bool match(const ExprPtr&target, Substitution& env) const override {
            if(env.find(name_) != env.end()){
                return env[name_]->is_equal(target);
            }
            env[name_] = target;
            return true;
        }
        ExprPtr substitute(const Substitution& env) const override {
            auto it = env.find(name_);
            if(it != env.end()){
                return it->second;
            }
            return std::make_shared<Wildcard>(name_);
        }
        ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
            return std::const_pointer_cast<Expr>(shared_from_this());
        }
        
};

// ---- Number ----
class Number : public Expr {
    double value_;
public:
    Number(double v) : value_(v) {}
    double value() const { return value_; }
    std::string to_string() const override { return std::to_string(value_); }
    ExprPtr derivative(const std::string&) const override { return std::make_shared<Number>(0.0); }
    ExprPtr simplify() const override { return std::const_pointer_cast<Expr>(shared_from_this()); }
    int precedence() const override { return 4; }
    std::string print(int, bool) const override {
        std::string s = std::to_string(value_);
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
        return s;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {}
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_num = std::dynamic_pointer_cast<Number>(target);
        return t_num && t_num->value() == value_;
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Number>(value_);        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    bool is_polynomical(const std::string& target_var) const override;
};
using NumberPtr = std::shared_ptr<Number>;

// ---- Variable ----
class Variable : public Expr {
    std::string name_;
public:
    Variable(const std::string& name) : name_(name) {}
    std::string to_string() const override { return name_; }
    ExprPtr derivative(const std::string& var) const override {
        return (name_ == var) ? std::make_shared<Number>(1.0) : std::make_shared<Number>(0.0);
    }
    ExprPtr simplify() const override { return std::const_pointer_cast<Expr>(shared_from_this()); }
    int precedence() const override { return 4; }
    std::string print(int, bool) const override { return name_; }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        auto it = vars.find(name_);
        if(it == vars.end()) throw std::runtime_error("Variable not found: " + name_);
        return std::make_shared<Number>(it->second);
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        out[name_] ++;
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_num = std::dynamic_pointer_cast<Variable>(target);
        return t_num && t_num->name_ == name_;
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Variable>(name_);        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
    bool is_polynomical(const std::string& target_var) const override;
};
using VariablePtr = std::shared_ptr<Variable>;

// ---- Forward declarations para Add, Mul, Sub, Div, Pow, etc ----
class Mul;
class Sub;
class Div;
class Pow;
class Sign;
class Sin;
class Cos;
class Tan;
class Exp;
class Log;
class Abs;
class Constant;

// ---- Add ----
class Add : public Expr {
public:
    ExprPtr left_, right_;
    Add(ExprPtr l, ExprPtr r) : left_(l), right_(r) {}
    std::string to_string() const override {
        return "(" + left_->to_string() + " + " + right_->to_string() + ")";
    }
    ExprPtr derivative(const std::string& var) const override;
    int precedence() const override { return 1; }
    std::string print(int parent_prec, bool) const override {
        std::string result = left_->print(1, true) + " + " + right_->print(1, false);
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr simplify() const override;
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        auto l = left_->evaluate(vars);
        auto r = right_->evaluate(vars);
        auto lnum = std::dynamic_pointer_cast<Number>(l);
        auto rnum = std::dynamic_pointer_cast<Number>(r);
        if (lnum && rnum) {
            return std::make_shared<Number>(lnum->value() + rnum->value())->simplify();
        }
        return std::make_shared<Add>(l, r)->simplify();
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        left_->collect_symbols(out);
        right_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Add>(target);
        if(!t_node) return false;
        
        std::vector<ExprPtr> target_terms;
        std::vector<ExprPtr> pattern_terms;
        flatten_add(target, target_terms);
        flatten_add(std::const_pointer_cast<Expr>(shared_from_this()), pattern_terms);

        if(pattern_terms.size() != target_terms.size()) {
            return false;
        }

        Substitution backup_env = env;

        std::vector<bool> used(target_terms.size(), false);

        for(const auto & p_term : pattern_terms){
            bool matched_term = false;

            for(size_t i = 0; i < target_terms.size(); i++){
                if(used[i]) continue;

                Substitution temp_env = backup_env;
                if(p_term->match(target_terms[i], temp_env)) {
                    backup_env = temp_env;
                    used[i] = true;
                    matched_term = true;
                    break;
                }
            }

            if(!matched_term) {
                return false;
            }
        }

        env = backup_env;
        return true;
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Add>(left_->substitute(env), right_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Add>(fn(left_), fn(right_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
    bool is_polynomical(const std::string& target_var) const override;
};

// ---- Mul ----
class Mul : public Expr {
public:
    ExprPtr left_, right_;
    Mul(ExprPtr l, ExprPtr r) : left_(l), right_(r) {}
    std::string to_string() const override {
        return "(" + left_->to_string() + " * " + right_->to_string() + ")";
    }
    ExprPtr derivative(const std::string& var) const override;
    int precedence() const override { return 2; }
    std::string print(int parent_prec, bool) const override {
        std::string result = left_->print(2, true) + " * " + right_->print(2, false);
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr simplify() const override;
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        auto l = left_->evaluate(vars);
        auto r = right_->evaluate(vars);
        auto lnum = std::dynamic_pointer_cast<Number>(l);
        auto rnum = std::dynamic_pointer_cast<Number>(r);
        if (lnum && rnum) {
            return std::make_shared<Number>(lnum->value() * rnum->value())->simplify();
        }
        return std::make_shared<Mul>(l, r)->simplify();
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        left_->collect_symbols(out);
        right_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Mul>(target);
        if(!t_node) return false;
        
        std::vector<ExprPtr> target_factors;
        std::vector<ExprPtr> pattern_factors;
        flatten_mul(target, target_factors);
        flatten_mul(std::const_pointer_cast<Expr>(shared_from_this()), pattern_factors);

        if(pattern_factors.size() != target_factors.size()) {
            return false;
        }

        Substitution backup_env = env;

        std::vector<bool> used(target_factors.size(), false);

        for(const auto & p_fact : pattern_factors){
            bool matched_term = false;

            for(size_t i = 0; i < target_factors.size(); i++){
                if(used[i]) continue;

                Substitution temp_env = backup_env;
                if(p_fact->match(target_factors[i], temp_env)) {
                    backup_env = temp_env;
                    used[i] = true;
                    matched_term = true;
                    break;
                }
            }

            if(!matched_term) {
                return false;
            }
        }

        env = backup_env;
        return true;
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Mul>(left_->substitute(env), right_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Mul>(fn(left_), fn(right_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
    bool is_polynomical(const std::string& target_var) const override;
};

// ---- Sub ----
class Sub : public Expr {
public:
    ExprPtr left_, right_;
    Sub(ExprPtr l, ExprPtr r) : left_(l), right_(r) {}
    std::string to_string() const override {
        return "(" + left_->to_string() + " - " + right_->to_string() + ")";
    }
    ExprPtr derivative(const std::string& var) const override;
    int precedence() const override { return 1; }
    std::string print(int parent_prec, bool) const override {
        std::string result = left_->print(1, true) + " - " + right_->print(1, false);
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr simplify() const override;
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        auto l = left_->evaluate(vars);
        auto r = right_->evaluate(vars);
        auto lnum = std::dynamic_pointer_cast<Number>(l);
        auto rnum = std::dynamic_pointer_cast<Number>(r);
        if (lnum && rnum) {
            return std::make_shared<Number>(lnum->value() - rnum->value());
        }
        return std::make_shared<Sub>(l, r);
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        left_->collect_symbols(out);
        right_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sub>(target);
        if(!t_node) return false;
        return left_->match(t_node->left_, env) && right_->match(t_node->right_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sub>(left_->substitute(env), right_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sub>(fn(left_), fn(right_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
    bool is_polynomical(const std::string& target_var) const override;
};

// ---- Div ----
class Div : public Expr {
public:
    ExprPtr left_, right_;
    Div(ExprPtr l, ExprPtr r) : left_(l), right_(r) {}
    std::string to_string() const override {
        return "(" + left_->to_string() + " / " + right_->to_string() + ")";
    }
    ExprPtr derivative(const std::string& var) const override;
    int precedence() const override { return 2; }
    std::string print(int parent_prec, bool) const override {
        std::string result = left_->print(2, true) + " / " + right_->print(2, false);
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr simplify() const override;
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        auto l = left_->evaluate(vars);
        auto r = right_->evaluate(vars);
        auto lnum = std::dynamic_pointer_cast<Number>(l);
        auto rnum = std::dynamic_pointer_cast<Number>(r);
        if (lnum && rnum) {
            return std::make_shared<Number>(lnum->value() / rnum->value());
        }
        return std::make_shared<Div>(l, r);
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        left_->collect_symbols(out);
        right_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Div>(target);
        if(!t_node) return false;
        return left_->match(t_node->left_, env) && right_->match(t_node->right_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Div>(left_->substitute(env), right_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Div>(fn(left_), fn(right_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
    bool is_polynomical(const std::string& target_var) const override;
};

// ---- Pow ----
class Pow : public Expr {
public:
    ExprPtr left_, right_;
    Pow(ExprPtr l, ExprPtr r) : left_(l), right_(r) {}
    std::string to_string() const override {
        return "(" + left_->to_string() + " ^ " + right_->to_string() + ")";
    }
    ExprPtr derivative(const std::string& var) const override;
    int precedence() const override { return 3; }
    std::string print(int parent_prec, bool) const override {
        std::string result = left_->print(3, true) + "^" + right_->print(4, false);
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr simplify() const override;
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        auto l = left_->evaluate(vars);
        auto r = right_->evaluate(vars);
        auto lnum = std::dynamic_pointer_cast<Number>(l);
        auto rnum = std::dynamic_pointer_cast<Number>(r);
        if (lnum && rnum) {
            return std::make_shared<Number>(std::pow(lnum->value(), rnum->value()));
        }
        return std::make_shared<Pow>(l, r);
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        left_->collect_symbols(out);
        right_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Pow>(target);
        if(!t_node) return false;
        return left_->match(t_node->left_, env) && right_->match(t_node->right_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Pow>(left_->substitute(env), right_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Pow>(fn(left_), fn(right_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
    bool is_polynomical(const std::string& target_var) const override;
};

// ---- Constant ----
class Constant : public Expr {
    std::string name_;
    double value_;
public:
    Constant(const std::string& name, double val) : name_(name), value_(val) {}
    std::string to_string() const override { return name_; }
    ExprPtr derivative(const std::string&) const override { return std::make_shared<Number>(0.0); }
    ExprPtr simplify() const override { return std::const_pointer_cast<Expr>(shared_from_this()); }
    int precedence() const override { return 4; }
    std::string print(int, bool) const override { return name_; }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Number>(value_);
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {}
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_num = std::dynamic_pointer_cast<Constant>(target);
        return t_num && t_num->name_ == name_;
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    bool is_polynomical(const std::string& target_var) const override;
};
using ConstantPtr = std::shared_ptr<Constant>;

class Infinite : public Expr {
public:
    Infinite(){}
    std::string to_string() const override { return "∞"; }
    ExprPtr derivative(const std::string&) const override { return std::make_shared<Number>(0.0); }
    ExprPtr simplify() const override { return std::const_pointer_cast<Expr>(shared_from_this()); }
    int precedence() const override { return 4; }
    std::string print(int, bool) const override { return "∞"; }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {}
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        return std::dynamic_pointer_cast<Infinite>(target) != nullptr;
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());     
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::const_pointer_cast<Expr>(shared_from_this());
    }
    bool is_polynomical(const std::string& target_var) const override;
};

ExprPtr make_number(double v);
ExprPtr make_variable(const std::string& name);
ExprPtr make_pi();
ExprPtr make_e();
ExprPtr make_inf();
ExprPtr make_i();

extern ExprPtr one;
extern ExprPtr zero;
extern ExprPtr two;
extern ExprPtr m_one;


// ============================================================
// Clase Abs
// ============================================================
class Abs : public Expr {
    ExprPtr arg_;
public:
    Abs(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "|" + arg_->to_string() + "|"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "|" + arg_->print(4, false) + "|";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Abs>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Abs>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Abs>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Abs>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};


class Sign : public Expr {
    ExprPtr arg_;
public:
    Sign(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "sign(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "sign(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Sign>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sign>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sign>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sign>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Exp ----
class Exp : public Expr {
    public:
    ExprPtr arg_;
    Exp(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "exp(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "exp(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Exp>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Exp>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Exp>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Exp>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Log ----
class Log : public Expr {
    public:
    ExprPtr arg_;
    Log(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "log(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "log(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Log>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Log>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Log>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Log>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

class Sqrt : public Expr {
    public:
    ExprPtr arg_;
    Sqrt(const ExprPtr& arg) : arg_(arg) {}

    std::string to_string() const override {
        return "sqrt(" + arg_->to_string() + ")";
    }

    ExprPtr derivative(const std::string& var) const override {
        // d/dx sqrt(u) = u' / (2 * sqrt(u))
        auto two = std::make_shared<Number>(2.0);
        return arg_->derivative(var)->mul(
            (two->mul(std::make_shared<Sqrt>(arg_)))->pow(make_number(-1.0))
        );
    }

    ExprPtr simplify() const override;
    int precedence() const override { return 4; }

    std::string print(int parent_prec, bool) const override {
        return "sqrt(" + arg_->print(4, false) + ")";
    }

    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Sqrt>(arg_->evaluate(vars));
    }

    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sqrt>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sqrt>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sqrt>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

class Root : public Expr {
public:
    ExprPtr index_;   // n
    ExprPtr radicand_; // x

    Root(const ExprPtr& idx, const ExprPtr& rad)
        : index_(idx), radicand_(rad) {}

    std::string to_string() const override {
        return "root(" + index_->to_string() + ", " + radicand_->to_string() + ")";
    }

    ExprPtr derivative(const std::string& var) const override;

    ExprPtr simplify() const override;

    int precedence() const override { return 4; }

    std::string print(int parent_prec, bool) const override {
        return "root(" + index_->print(4, false) + ", " + radicand_->print(4, false) + ")";
    }

    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Root>(index_->evaluate(vars), radicand_->evaluate(vars));
    }

    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        index_->collect_symbols(out);
        radicand_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Root>(target);
        if(!t_node) return false;
        
        return index_->match(t_node->index_, env) && radicand_->match(t_node->radicand_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Root>(index_->substitute(env), radicand_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Root>(fn(index_), fn(radicand_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

class LogBase : public Expr {
public:
    ExprPtr base_;
    ExprPtr arg_;

    LogBase(const ExprPtr& base, const ExprPtr& arg)
        : base_(base), arg_(arg) {}

    std::string to_string() const override {
        return "log_" + base_->to_string() + "(" + arg_->to_string() + ")";
    }

    ExprPtr derivative(const std::string& var) const override {
        // d/dx log_b(u) = u' / (u * ln(b))
        auto ln_b = std::make_shared<Log>(base_);
        auto denom = std::make_shared<Mul>(arg_, ln_b);
        return arg_->derivative(var)->mul(denom->pow(make_number(-1.0)));
    }

    ExprPtr simplify() const override;
    int precedence() const override { return 4; }

    std::string print(int parent_prec, bool) const override {
        return "log_" + base_->print(4, false) + "(" + arg_->print(4, false) + ")";
    }

    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<LogBase>(base_->evaluate(vars), arg_->evaluate(vars));
    }

    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        base_->collect_symbols(out);
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<LogBase>(target);
        if(!t_node) return false;
        
        return base_->match(t_node->base_, env) && arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<LogBase>(base_->substitute(env), arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<LogBase>(fn(base_), fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Sin ----
class Sin : public Expr {
    public:
    ExprPtr arg_;
    Sin(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "sin(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "sin(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Sin>(arg_->evaluate(vars))->simplify();
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sin>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sin>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sin>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Cos ----
class Cos : public Expr {
    public:
    ExprPtr arg_;
    Cos(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "cos(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "cos(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Cos>(arg_->evaluate(vars))->simplify();
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Cos>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Cos>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Cos>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Tan ----
class Tan : public Expr {
    public:
    ExprPtr arg_;
    Tan(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "tan(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "tan(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Tan>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Tan>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Tan>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Tan>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Sec ----
class Sec : public Expr {
    public:
    ExprPtr arg_;
    Sec(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "sec" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "sec" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Sec>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sec>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sec>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sec>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Csc ----
class Csc : public Expr {
    public:
    ExprPtr arg_;
    Csc(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "csc(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "csc(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Csc>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Csc>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Csc>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Csc>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Cot ----
class Cot : public Expr {
    public:
    ExprPtr arg_;
    Cot(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "cot(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "cot(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Cot>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Cot>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Cot>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Cot>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

class Arcsin : public Expr {
    public:
    ExprPtr arg_;
    Arcsin(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arcsin(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "arcsin(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arcsin>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arcsin>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arcsin>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arcsin>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Arccos ----
class Arccos : public Expr {
    public:
    ExprPtr arg_;
    Arccos(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arccos(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "arccos(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arccos>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arccos>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arccos>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arccos>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Arctan ----
class Arctan : public Expr {
    public:
    ExprPtr arg_;
    Arctan(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arctan(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "arctan(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arctan>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arctan>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arctan>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arctan>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Arcsec ----
class Arcsec : public Expr {
    public:
    ExprPtr arg_;
    Arcsec(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arcsec(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "arcsec(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arcsec>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arcsec>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arcsec>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arcsec>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Arccsc ----
class Arccsc : public Expr {
    public:
    ExprPtr arg_;
    Arccsc(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arccsc(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "arccsc(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arccsc>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arccsc>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arccsc>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arccsc>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ---- Arccot ----
class Arccot : public Expr {
    public:
    ExprPtr arg_;
    Arccot(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arccot(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        std::string result = "arccot(" + arg_->print(4, false) + ")";
        if (precedence() < parent_prec) return "(" + result + ")";
        return result;
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arccot>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arccot>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arccot>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arccot>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Sinh
// ============================================================
class Sinh : public Expr {
    public:
    ExprPtr arg_;
    Sinh(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "sinh(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "sinh(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Sinh>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sinh>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sinh>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sinh>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Cosh
// ============================================================
class Cosh : public Expr {
    public:
    ExprPtr arg_;
    Cosh(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "cosh(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "cosh(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Cosh>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Cosh>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Cosh>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Cosh>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Tanh
// ============================================================
class Tanh : public Expr {
    public:
    ExprPtr arg_;
    Tanh(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "tanh(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "tanh(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Tanh>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Tanh>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Tanh>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Tanh>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Coth
// ============================================================
class Coth : public Expr {
    public:
    ExprPtr arg_;
    Coth(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "coth(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "coth(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Coth>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Coth>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Coth>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Coth>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Sech
// ============================================================
class Sech : public Expr {
    public:
    ExprPtr arg_;
    Sech(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "sech(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "sech(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Sech>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Sech>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Sech>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Sech>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Csch
// ============================================================
class Csch : public Expr {
    public:
    ExprPtr arg_;
    Csch(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "csch(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "csch(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Csch>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Csch>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Csch>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Csch>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Arcsinh
// ============================================================
class Arcsinh : public Expr {
    public:
    ExprPtr arg_;
    Arcsinh(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arcsinh(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "arcsinh(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arcsinh>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arcsinh>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arcsinh>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arcsinh>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Arccosh
// ============================================================
class Arccosh : public Expr {
    public:
    ExprPtr arg_;
    Arccosh(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arccosh(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "arccosh(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arccosh>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arccosh>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arccosh>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arccosh>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Arctanh
// ============================================================
class Arctanh : public Expr {
    public:
    ExprPtr arg_;
    Arctanh(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arctanh(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "arctanh(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arctanh>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arctanh>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arctanh>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arctanh>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Arccoth
// ============================================================
class Arccoth : public Expr {
    public:
    ExprPtr arg_;
    Arccoth(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arccoth(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "arccoth(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arccoth>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arccoth>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arccoth>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arccoth>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Arcsech
// ============================================================
class Arcsech : public Expr {
    public:
    ExprPtr arg_;
    Arcsech(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arcsech(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "arcsech(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arcsech>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arcsech>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arcsech>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arcsech>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

// ============================================================
// Arccsch
// ============================================================
class Arccsch : public Expr {
    public:
    ExprPtr arg_;
    Arccsch(const ExprPtr& arg) : arg_(arg) {}
    std::string to_string() const override { return "arccsch(" + arg_->to_string() + ")"; }
    ExprPtr derivative(const std::string& var) const override;
    ExprPtr simplify() const override;
    int precedence() const override { return 4; }
    std::string print(int parent_prec, bool) const override {
        return "arccsch(" + arg_->print(4, false) + ")";
    }
    ExprPtr evaluate(const std::map<std::string, double>& vars) const override {
        return std::make_shared<Arccsch>(arg_->evaluate(vars));
    }
    void collect_symbols(std::unordered_map<std::string, int>& out) const override {
        arg_->collect_symbols(out);
    }
    int type_id() const override;
    int compare_same_type(const ExprPtr&other) const override;
    ExprPtr normalize() const override;
    bool match(const ExprPtr& target, Substitution& env) const override {
        auto t_node = std::dynamic_pointer_cast<Arccsch>(target);
        if(!t_node) return false;
        
        return arg_->match(t_node->arg_, env);
    }
    ExprPtr substitute(const Substitution& env) const override {
        return std::make_shared<Arccsch>(arg_->substitute(env));        
    }
    ExprPtr transform_children(const std::function<ExprPtr(const ExprPtr&)> &fn) const override {
        return std::make_shared<Arccsch>(fn(arg_));
    }
    ExprPtr isolate(const std::string& target_var, ExprPtr rhs) const override;
};

class ImaginaryUnit : public Expr {
    public:

        ImaginaryUnit(){}

        std::string to_string() const override {
            return "i";
        }
        std::string print(int parent_prec, bool) const override {
            return "i";
        }

        int type_id() const override;
};

ExprPtr factorize(const ExprPtr& expr);

ExprPtr expand(const ExprPtr& expr);

#endif EXPR_H