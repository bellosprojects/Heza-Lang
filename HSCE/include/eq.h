#pragma once

#define EQ_H
#ifdef EQ_H

#define _USE_MATH_DEFINES
#include <memory>
#include <string>
#include <pybind11/pybind11.h>
#include "expr.h"

namespace py = pybind11;

class Equation {
    private:
        ExprPtr lhs_;
        ExprPtr rhs_; 

    public:
        Equation(const ExprPtr& lhs, const ExprPtr& rhs);

        ExprPtr get_lhs() const { return lhs_; }
        ExprPtr get_rhs() const { return rhs_; }

        std::string to_string() const;
        ExprPtr to_standard_form() const;
        std::set<ExprPtr> solve(const ExprPtr& var) const;
};

/**
 * @brief Obtiene los coeficientes de una expresion polimonica
 * 
 * @param e Expresion a descomponer
 * @return std::pair<bool, std::unordered_map<int, ExprPtr>> Mapa de la forma {grado: exponente}
 */
std::pair<bool, std::unordered_map<int, ExprPtr>> coeficientes(const ExprPtr &e, const std::string& target_var);

std::vector<ExprPtr> resolvente(const ExprPtr& a, const ExprPtr& b, const ExprPtr& c);

int grade_of(const std::unordered_map<int, ExprPtr>& coeff);

#endif EQ_H