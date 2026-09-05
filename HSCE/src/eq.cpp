#include "eq.h"
#include <memory>

Equation::Equation(const ExprPtr& lhs, const ExprPtr& rhs)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

std::string Equation::to_string() const {
    return lhs_->prints() + " = " + rhs_->prints(); 
}

ExprPtr Equation::to_standard_form() const {
    return std::make_shared<Sub>(lhs_, rhs_)->simplify();
}

std::set<ExprPtr> Equation::solve(const ExprPtr& var) const {

    std::string var_name;
    if(auto var_ = std::dynamic_pointer_cast<Variable>(var)){
        var_name = var_->to_string();
    } else {
        throw std::runtime_error("El objeto a despejar debe ser una variable.");
    }

    std::set<ExprPtr> solutions;

    auto diff = to_standard_form();
    auto expanded = expand(diff);
    auto normalize = expanded->normalize();
    auto factorized = factorize(normalize);

    
    std::vector<ExprPtr> factors;
    flatten_mul(factorized, factors);
    
    for(auto& factor: factors){
        factor = expand(factor);
        double val;
        if(is_number(factor, val)){
            continue;
        }

        // Encontrar raices
        auto symbols = factor->symbols();
        int ocurrencias = symbols[var_name];
        bool solved = false;


        // Es polinomica
        if(factor->is_polynomical(var_name)){

            
            auto grado_coeficiente = coeficientes(factor, var_name);
            if(grado_coeficiente.first){
                
                int grade = grade_of(grado_coeficiente.second);
                
                if(grade == 2){
                    
                    auto coeff_map = grado_coeficiente.second;

                    auto a_coef = (coeff_map.find(2) != coeff_map.end()) ? coeff_map[2] : zero;
                    auto b_coef = (coeff_map.find(1) != coeff_map.end()) ? coeff_map[1] : zero;
                    auto c_coef = (coeff_map.find(0) != coeff_map.end()) ? coeff_map[0] : zero;

                    auto cuadratic_roots = resolvente(a_coef, b_coef, c_coef);
                    for(const auto&r: cuadratic_roots){
                        solutions.insert(r);
                    }

                    solved = true;

                }

            }

        } 

        if(ocurrencias == 1 && !solved){
            auto root = factor->isolate(var_name, zero)->simplify();
            solutions.insert(root);
        }

    }

    return solutions;
}

std::vector<ExprPtr> resolvente(const ExprPtr& a, const ExprPtr& b, const ExprPtr& c) {
    std::vector<ExprPtr> roots;

    // Constantes útiles
    auto zero = std::make_shared<Number>(0.0);
    auto one = std::make_shared<Number>(1.0);
    auto two = std::make_shared<Number>(2.0);
    auto four = std::make_shared<Number>(4.0);

    // b^2 -> Pow(b, 2)
    auto b_squared = std::make_shared<Pow>(b, two);

    // 4 * a * c -> Mul(Mul(4, a), c)
    auto four_ac = std::make_shared<Mul>(std::make_shared<Mul>(four, a), c);

    // Discriminante: b^2 - 4ac -> Sub(b_squared, four_ac)
    auto discriminant = std::make_shared<Sub>(b_squared, four_ac)->simplify();

    // Raíz cuadrada del discriminante -> Sqrt(discriminant)
    auto sqrt_disc = std::make_shared<Sqrt>(discriminant)->simplify();

    // -b -> Sub(0, b) o Mul(Number(-1), b)
    auto minus_b = std::make_shared<Sub>(zero, b)->simplify();

    // 2 * a -> Mul(2, a)
    auto two_a = std::make_shared<Mul>(two, a)->simplify();

    // Raíz 1: (-b + sqrt_disc) / (2 * a)
    auto num_plus = std::make_shared<Add>(minus_b, sqrt_disc)->simplify();
    auto root1 = std::make_shared<Div>(num_plus, two_a)->simplify();

    // Raíz 2: (-b - sqrt_disc) / (2 * a)
    auto num_minus = std::make_shared<Sub>(minus_b, sqrt_disc)->simplify();
    auto root2 = std::make_shared<Div>(num_minus, two_a)->simplify();

    roots.push_back(root1);
    roots.push_back(root2);

    return roots;
}

int grade_of(const std::unordered_map<int, ExprPtr>& coeff) {
    if (coeff.empty()) {
        return 0;
    }

    int max_grade = -1;

    for (const auto& pair : coeff) {
        if (pair.first > max_grade) {
            max_grade = pair.first;
        }
    }

    return max_grade;
}

std::pair<bool, std::unordered_map<int, ExprPtr>> coeficientes(const ExprPtr &e, const std::string& target_var){
    std::vector<ExprPtr> terms;
    flatten_add(e->simplify(), terms);

    std::unordered_map<int, ExprPtr> coeff;
    bool valid = true;

    auto COE_WILCARD = std::make_shared<Wildcard>("COE");
    auto EXP_WILCARD = std::make_shared<Wildcard>("EXP");

    auto pattern1 = std::make_shared<Mul>(
        COE_WILCARD,
        std::make_shared<Pow>(
            std::make_shared<Variable>(target_var),
            EXP_WILCARD
        )
    );

    auto pattern2 = std::make_shared<Pow>(
        std::make_shared<Variable>(target_var),
        EXP_WILCARD
    );

    auto pattern3 = std::make_shared<Mul>(
        COE_WILCARD,
        std::make_shared<Variable>(target_var)
    );

    auto pattern4 = std::make_shared<Variable>(target_var);
    auto pattern5 = COE_WILCARD;

    for(const auto& t: terms){
        Substitution sub1{}, sub2{}, sub3{}, sub4{}, sub5{};

        if(pattern1->match(t, sub1)){
            auto coe = sub1.find("COE");
            auto exp = sub1.find("EXP");

            if(coe != sub1.end() && exp != sub1.end()){
                auto cleared_coeff = coe->second;
                auto cleared_exp = exp->second->evaluate({})->simplify();

                double num_exp;
                if(is_number(cleared_exp, num_exp)){
                    if(num_exp == std::floor(num_exp)){
                        int exp_int = static_cast<int>(num_exp);
                        if(coeff.find(exp_int) != coeff.end()){
                            coeff[exp_int] = std::make_shared<Add>(coeff[exp_int], cleared_coeff)->simplify();
                        } else {
                            coeff[exp_int] = cleared_coeff;
                        }
                    } else {
                        valid = false; break;
                    }
                } else {
                    valid = false; break;
                }
            }
        }
        else if(pattern2->match(t, sub2)){
            auto exp = sub2.find("EXP");
            if(exp != sub2.end()){
                auto cleared_exp = exp->second->evaluate({})->simplify();

                double num_exp;
                if(is_number(cleared_exp, num_exp)){
                    if(num_exp == std::floor(num_exp)){
                        int exp_int = static_cast<int>(num_exp);
                        if(coeff.find(exp_int) != coeff.end()){
                            coeff[exp_int] = std::make_shared<Add>(coeff[exp_int], one)->simplify();
                        } else {
                            coeff[exp_int] = one;
                        }
                    } else {
                        valid = false; break;
                    }
                } else {
                    valid = false; break;
                }
            }
        }
        else if(pattern3->match(t, sub3)){
            auto coe = sub3.find("COE");
            if(coe != sub3.end()){
                auto cleared_coeff = coe->second;
                if(coeff.find(1) != coeff.end()){
                    coeff[1] = std::make_shared<Add>(coeff[1], cleared_coeff)->simplify();
                } else {
                    coeff[1] = cleared_coeff;
                }
            }
        }
        else if(pattern4->match(t, sub4)){ 
            if(coeff.find(1) != coeff.end()){
                coeff[1] = std::make_shared<Add>(coeff[1], one)->simplify();
            } else {
                coeff[1] = one;
            }
        }
        else if(pattern5->match(t, sub5)){
            auto coe = sub5.find("COE");
            if(coe != sub5.end()){
                auto cleared_coeff = coe->second;
                if(coeff.find(0) != coeff.end()){
                    coeff[0] = std::make_shared<Add>(coeff[0], cleared_coeff)->simplify();
                } else {
                    coeff[0] = cleared_coeff;
                }
            }
        } else {
            valid = false;
            break;
        }
    }

    return std::pair(valid, coeff);
}