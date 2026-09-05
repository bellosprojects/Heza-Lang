#define _USE_MATH_DEFINES
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <vector>
#include <string>
#include <cmath>
#include "expr.h"
#include "eq.h"

namespace py = pybind11;

// ---- Módulo Python ----
PYBIND11_MODULE(HSCE, m) {
    m.doc() = R"pbdoc(
        HSCE (Heza Symbolic Calculation Engine)
        ---------------------------------------
        Un Motor de Cálculo Simbólico en C++ con enlaces optimizados para Python,
        diseñado para la manipulación de árboles de expresiones algebraicas,
        coincidencia de patrones (pattern matching) y reescritura de reglas.
    )pbdoc";

    // ==========================================
    // CLASE BASE: Expr
    // ==========================================
    py::class_<Expr, ExprPtr>(m, "Expr", "Clase base abstracta para todas las expresiones simbólicas del árbol AST. ")
        .def("to_string", &Expr::to_string, 
             "Convierte la expresión a una cadena de texto sin simplificaciones forzosas de paréntesis.")
        .def("print", &Expr::prints, 
             "Devuelve una representación textual limpia y simplificada de la expresión.")
        .def("derivative", &Expr::derivative, py::arg("var"),
             "Calcula la derivada analítica de la expresión con respecto a la variable especificada.")
        .def("simplify", &Expr::simplify, 
             "Simplifica la expresión utilizando ordenamiento canónico, agrupación de términos y leyes de exponentes.")
        .def("evaluate", &Expr::evaluate, py::arg("vars"),
             "Evalúa numéricamente la expresión sustituyendo las variables libres por los valores del diccionario proporcionado.")
        .def("symbols", &Expr::symbols, 
             "Retorna un conjunto con los nombres de todas las variables libres contenidas en la expresión.")
        .def("number", &Expr::number, 
             "Convierte el nodo a un valor de tipo float. Lanza un error si la expresión aún contiene variables sin evaluar.")
        .def("equals", &Expr::is_equal, py::arg("other"),
             "Verifica si dos árboles de expresiones son estructural e idénticamente iguales usando comparación canónica.")
        .def("__str__", &Expr::prints)
        .def("__repr__", &Expr::prints)
        .def("__add__", &Expr::add)
        .def("__add__", &Expr::add_double)
        .def("__radd__", &Expr::add_double)
        .def("__mul__", &Expr::mul)
        .def("__mul__", &Expr::mul_double)
        .def("__rmul__", &Expr::mul_double)
        .def("__sub__", &Expr::sub)
        .def("__sub__", &Expr::sub_double)
        .def("__rsub__", &Expr::rsub_double)
        .def("__truediv__", &Expr::div)
        .def("__truediv__", &Expr::div_double)
        .def("__rtruediv__", &Expr::rdiv_double)
        .def("__pow__", &Expr::pow)
        .def("__pow__", &Expr::pow_double)
        .def("__rpow__", &Expr::rpow_double);

    // ==========================================
    // NODOS HOJA Y PATRONES
    // ==========================================
    py::class_<Number, Expr, NumberPtr>(m, "Number", "Nodo hoja que representa una constante numérica real.")
        .def(py::init<double>(), py::arg("value"), "Inicializa un número con un valor de tipo double.")
        .def("value", &Number::value, "Retorna el valor numérico interno.");

    py::class_<Variable, Expr, VariablePtr>(m, "Variable", "Objeto simbólico que representa una variable matemática (ej. 'x', 'y').")
        .def(py::init<const std::string&>(), py::arg("name"), "Inicializa una variable con su respectivo nombre identificador.");

    py::class_<Wildcard, Expr, std::shared_ptr<Wildcard>>(m, "Wildcard", "Comodín para motores de búsqueda de patrones algebraicos.")
        .def(py::init<const std::string&>(), py::arg("name"), "Inicializa un comodín con una etiqueta de captura (ej. 'X').");

    // ==========================================
    // MOTOR DE REESCRITURA
    // ==========================================
    py::class_<RewriteRule, std::shared_ptr<RewriteRule>>(m, "RewriteRule", "Encapsula una regla de transformación algebraica basada en un patrón y un reemplazo.")
        .def(py::init<const ExprPtr&, const ExprPtr&>(), py::arg("pattern"), py::arg("replacement"), 
             "Crea una regla de reescritura que transforma un patrón encontrado en una estructura de reemplazo.")
        .def("apply", &RewriteRule::apply, py::arg("expr"), 
             "Intenta aplicar la regla a una expresión objetivo; retorna la expresión transformada o None si no hay coincidencia.");

    py::class_<RewriteEngine, std::shared_ptr<RewriteEngine>>(m, "RewriteEngine", "Motor inteligente encargado de aplicar un conjunto de reglas de reescritura de forma iterativa.")
        .def(py::init<std::vector<RewriteRule>&>(), py::arg("rules"), 
             "Inicializa el motor con una lista de reglas de reescritura.")
        .def("add_rule", &RewriteEngine::add_rule, py::arg("rule"), 
             "Agrega dinámicamente una nueva regla de reescritura al motor.")
        .def("simplify", &RewriteEngine::simplify, py::arg("expr"), 
             "Ejecuta el bucle de punto fijo combinando simplificación canónica nativa y reglas del motor hasta estabilizar la expresión.");

    // ==========================================
    // FUNCIONES GLOBALES
    // ==========================================
    m.def("match_pattern", [](const ExprPtr& target, const ExprPtr& pattern) {
        Substitution env;
        if (pattern->match(target->simplify(), env)){
            return env;
        }
        return Substitution{};
    }, py::arg("target"), py::arg("pattern"),
       R"pbdoc(
           Compara una expresión objetivo con un patrón que contiene comodines.
           
           Args:
               target: La expresión real del AST a evaluar.
               pattern: El patrón estructural con comodines (Wildcards).
               
           Returns:
               Un diccionario (Substitution) con las asignaciones clave-valor si hay coincidencia, 
               o un diccionario vacío si falla.
       )pbdoc");

    m.def("factorize", &factorize, py::arg("expr"), 
    "Aplica el motor de reescritura con reglas de factorizacion algebraica a una expresion.");

    m.def("expand", &expand, py::arg("expr"), 
    "Aplica el motor de reescritura con reglas de expansion algebraica a una expresion.");

    m.def("coeff", &coeficientes, py::arg("expr"), py::arg("target_var"),
    "Extrae los coeficientes de una expresion polinomica de la forma {grado: coeficiente}");

    // ==========================================
    // FUNCIONES TRIGONOMÉTRICAS Y UNARIAS
    // ==========================================
    #define BIND_UNARY_NODE(ClassPython, ClassCpp) \
        py::class_<ClassCpp, Expr, std::shared_ptr<ClassCpp>>(m, #ClassPython, "Nodo unario correspondiente a la función " #ClassPython ".") \
            .def(py::init<const ExprPtr&>(), py::arg("arg"), "Inicializa la función con un argumento interno.")

    BIND_UNARY_NODE(Sin, Sin);
    BIND_UNARY_NODE(Cos, Cos);
    BIND_UNARY_NODE(Tan, Tan);
    BIND_UNARY_NODE(Sec, Sec);
    BIND_UNARY_NODE(Csc, Csc);
    BIND_UNARY_NODE(Cot, Cot);
    BIND_UNARY_NODE(Arcsin, Arcsin);
    BIND_UNARY_NODE(Arccos, Arccos);
    BIND_UNARY_NODE(Arctan, Arctan);
    BIND_UNARY_NODE(Arcsec, Arcsec);
    BIND_UNARY_NODE(Arccsc, Arccsc);
    BIND_UNARY_NODE(Arccot, Arccot);
    
    BIND_UNARY_NODE(Sinh, Sinh);
    BIND_UNARY_NODE(Cosh, Cosh);
    BIND_UNARY_NODE(Tanh, Tanh);
    BIND_UNARY_NODE(Sech, Sech);
    BIND_UNARY_NODE(Csch, Csch);
    BIND_UNARY_NODE(Coth, Coth);
    BIND_UNARY_NODE(Arcsinh, Arcsinh);
    BIND_UNARY_NODE(Arccosh, Arccosh);
    BIND_UNARY_NODE(Arctanh, Arctanh);
    BIND_UNARY_NODE(Arcsech, Arcsech);
    BIND_UNARY_NODE(Arccsch, Arccsch);
    BIND_UNARY_NODE(Arccoth, Arccoth);

    BIND_UNARY_NODE(Exp, Exp);
    BIND_UNARY_NODE(Log, Log);
    BIND_UNARY_NODE(Abs, Abs);
    BIND_UNARY_NODE(Sign, Sign);
    BIND_UNARY_NODE(Sqrt, Sqrt);

    // ==========================================
    // NODOS BINARIOS ESPECIALES
    // ==========================================
    py::class_<Root, Expr, std::shared_ptr<Root>>(m, "Root", "Representa una raíz de índice arbitrario (ej. raíz n-ésima).")
        .def(py::init<const ExprPtr&, const ExprPtr&>(), py::arg("index"), py::arg("arg"), 
            "Inicializa la raíz con su índice y el radicando.");

    py::class_<LogBase, Expr, std::shared_ptr<LogBase>>(m, "LogBase", "Representa un logaritmo con base arbitraria.")
        .def(py::init<const ExprPtr&, const ExprPtr&>(), py::arg("base"), py::arg("arg"), 
            "Inicializa el logaritmo con su base y su argumento.");

    // ==========================================
    // UTILIDADES Y CONSTANTES GLOBALES
    // ==========================================
    m.def("make_number", &make_number, py::arg("value"), "Crea y retorna un nodo constante numérico.");
    m.def("make_symbol", &make_variable, py::arg("name"), "Crea y retorna un nodo de variable simbólica.");
    
    m.attr("pi") = make_pi();
    m.attr("e") = make_e();
    m.attr("inf") = make_inf();
    m.attr("i") = make_i();

    py::class_<Equation, std::shared_ptr<Equation>>(m, "Equation", "Representacion de una ecuacion matematica")
        .def(py::init<const ExprPtr&, const ExprPtr&>(), py::arg("lhs"), py::arg("rhs"), "Inicializa la ecuacion")
        .def("__str__", &Equation::to_string)
        .def("solve", &Equation::solve, "Resuelve la ecuacion para una variable dada.");
}


// py::class_<Set, SetPtr>(m, "HezaSet")
//     .def("contains", &Set::contains, "Verifica si el elemento existe dentro del conjunto")
//     .def("values", &Set::print, "Lista los primeros 100 valores que puede contener el conjunto")
//     .def("union", &Set::union_, "Union de conjuntos")
//     .def("intersection", &Set::intersection, "Interseccion de conjuntos")
//     .def("difference", &Set::difference, "Diferencia de conjuntos")
//     .def("symetric_difference", &Set::symetric_difference, "Diferencia simetrica de conjuntos")
//     .def("complement", &Set::complement, "Obtiene el complemento usando universe como referencia")
//     .def("complement", &Set::complement_self, "Obtiene el complemento usando el universo inferido (usar con precausion)");

// py::class_<SetFinite, Set, std::shared_ptr<SetFinite>>(m, "FiniteSet")
//     .def(py::init<const std::set<double>&>());

// py::class_<SetEmpty, Set, std::shared_ptr<SetEmpty>>(m, "EmptySet")
//     .def(py::init());

// py::class_<Interval, Set, std::shared_ptr<Interval>>(m, "Interval")
//     .def(py::init<double, double, bool, bool>()) 
//     .def(py::init<double, double>());  
// m.attr("reals") = make_reals();