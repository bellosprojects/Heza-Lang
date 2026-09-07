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
#include <heza/heza.hpp>

namespace py = pybind11;

// ---- Módulo Python ----
PYBIND11_MODULE(HSCE, m) {
    m.doc() = R"pbdoc(
        HSCE (Heza Symbolic Calculation Engine)
        ---------------------------------------
        Un Motor de Cálculo Simbólico en C++ con enlaces optimizados para Python,
        diseñado para la manipulación de árboles de Expresiones algebraicas,
        coincidencia de patrones (pattern matching) y reescritura de reglas.
    )pbdoc";

    // ==========================================
    // CLASE BASE: heza::core::Expr
    // ==========================================
    py::class_<heza::core::Expr, heza::core::ExprPtr>(m, "Expr", "Clase base abstracta para todas las Expresiones simbólicas del árbol AST. ")
        .def("to_string", &heza::core::Expr::to_string, 
             "Convierte la heza::core::Expresión a una cadena de texto sin simplificaciones forzosas de paréntesis.")
        .def("print", &heza::core::Expr::prints, 
             "Devuelve una representación textual limpia y simplificada de la Expresión.")
        .def("derivative", &heza::core::Expr::derivative, py::arg("var"),
             "Calcula la derivada analítica de la heza::core::Expresión con respecto a la variable especificada.")
        .def("simplify", &heza::core::Expr::simplify, 
             "Simplifica la heza::core::Expresión utilizando ordenamiento canónico, agrupación de términos y leyes de exponentes.")
        .def("evaluate", &heza::core::Expr::evaluate, py::arg("vars"),
             "Evalúa numéricamente la heza::core::Expresión sustituyendo las variables libres por los valores del diccionario proporcionado.")
        .def("symbols", &heza::core::Expr::symbols, 
             "Retorna un conjunto con los nombres de todas las variables libres contenidas en la heza::core::Expresión.")
        .def("number", &heza::core::Expr::number, 
             "Convierte el nodo a un valor de tipo float. Lanza un error si la heza::core::Expresión aún contiene variables sin evaluar.")
        .def("equals", &heza::core::Expr::is_equal, py::arg("other"),
             "Verifica si dos árboles de heza::core::Expresiones son estructural e idénticamente iguales usando comparación canónica.")
        .def("__str__", &heza::core::Expr::prints)
        .def("__repr__", &heza::core::Expr::prints)
        .def("__add__", &heza::core::Expr::add)
        .def("__add__", &heza::core::Expr::add_double)
        .def("__radd__", &heza::core::Expr::add_double)
        .def("__mul__", &heza::core::Expr::mul)
        .def("__mul__", &heza::core::Expr::mul_double)
        .def("__rmul__", &heza::core::Expr::mul_double)
        .def("__sub__", &heza::core::Expr::sub)
        .def("__sub__", &heza::core::Expr::sub_double)
        .def("__rsub__", &heza::core::Expr::rsub_double)
        .def("__truediv__", &heza::core::Expr::div)
        .def("__truediv__", &heza::core::Expr::div_double)
        .def("__rtruediv__", &heza::core::Expr::rdiv_double)
        .def("__pow__", &heza::core::Expr::pow)
        .def("__pow__", &heza::core::Expr::pow_double)
        .def("__rpow__", &heza::core::Expr::rpow_double);

    // ==========================================
    // NODOS HOJA Y PATRONES
    // ==========================================
    py::class_<heza::core::Number, heza::core::Expr, heza::core::NumberPtr>(m, "Number", "Nodo hoja que representa una constante numérica real.")
        .def(py::init<double>(), py::arg("value"), "Inicializa un número con un valor de tipo double.")
        .def("value", &heza::core::Number::value, "Retorna el valor numérico interno.");

    py::class_<heza::core::Variable, heza::core::Expr, heza::core::VariablePtr>(m, "Variable", "Objeto simbólico que representa una variable matemática (ej. 'x', 'y').")
        .def(py::init<const std::string&>(), py::arg("name"), "Inicializa una variable con su respectivo nombre identificador.");

    py::class_<heza::core::Wildcard, heza::core::Expr, std::shared_ptr<heza::core::Wildcard>>(m, "Wildcard", "Comodín para motores de búsqueda de patrones algebraicos.")
        .def(py::init<const std::string&>(), py::arg("name"), "Inicializa un comodín con una etiqueta de captura (ej. 'X').");

    // ==========================================
    // MOTOR DE REESCRITURA
    // ==========================================
    py::class_<heza::core::RewriteRule, std::shared_ptr<heza::core::RewriteRule>>(m, "RewriteRule", "Encapsula una regla de transformación algebraica basada en un patrón y un reemplazo.")
        .def(py::init<const heza::core::ExprPtr&, const heza::core::ExprPtr&>(), py::arg("pattern"), py::arg("replacement"), 
             "Crea una regla de reescritura que transforma un patrón encontrado en una estructura de reemplazo.")
        .def("apply", &heza::core::RewriteRule::apply, py::arg("heza::core::Expr"), 
             "Intenta aplicar la regla a una heza::core::Expresión objetivo; retorna la heza::core::Expresión transformada o None si no hay coincidencia.");

    py::class_<heza::core::RewriteEngine, std::shared_ptr<heza::core::RewriteEngine>>(m, "RewriteEngine", "Motor inteligente encargado de aplicar un conjunto de reglas de reescritura de forma iterativa.")
        .def(py::init<std::vector<heza::core::RewriteRule>&>(), py::arg("rules"), 
             "Inicializa el motor con una lista de reglas de reescritura.")
        .def("add_rule", &heza::core::RewriteEngine::add_rule, py::arg("rule"), 
             "Agrega dinámicamente una nueva regla de reescritura al motor.")
        .def("simplify", &heza::core::RewriteEngine::simplify, py::arg("heza::core::Expr"), 
             "Ejecuta el bucle de punto fijo combinando simplificación canónica nativa y reglas del motor hasta estabilizar la heza::core::Expresión.");

    // ==========================================
    // FUNCIONES GLOBALES
    // ==========================================
    m.def("match_pattern", [](const heza::core::ExprPtr& target, const heza::core::ExprPtr& pattern) {
        heza::core::Substitution env;
        if (pattern->match(target->simplify(), env)){
            return env;
        }
        return heza::core::Substitution{};
    }, py::arg("target"), py::arg("pattern"),
       R"pbdoc(
           Compara una heza::core::Expresión objetivo con un patrón que contiene comodines.
           
           Args:
               target: La heza::core::Expresión real del AST a evaluar.
               pattern: El patrón estructural con comodines (Wildcards).
               
           Returns:
               Un diccionario (Substitution) con las asignaciones clave-valor si hay coincidencia, 
               o un diccionario vacío si falla.
       )pbdoc");

    m.def("factorize", &heza::core::factorize, py::arg("heza::core::Expr"), 
    "Aplica el motor de reescritura con reglas de factorizacion algebraica a una heza::core::Expresion.");

    m.def("expand", &heza::core::expand, py::arg("heza::core::Expr"), 
    "Aplica el motor de reescritura con reglas de expansion algebraica a una heza::core::Expresion.");

    // ==========================================
    // FUNCIONES TRIGONOMÉTRICAS Y UNARIAS
    // ==========================================
    #define BIND_UNARY_NODE(ClassPython, ClassCpp) \
        py::class_<ClassCpp, heza::core::Expr, std::shared_ptr<ClassCpp>>(m, #ClassPython, "Nodo unario correspondiente a la función " #ClassPython ".") \
            .def(py::init<const heza::core::ExprPtr&>(), py::arg("arg"), "Inicializa la función con un argumento interno.")

    BIND_UNARY_NODE(Sin, heza::core::Sin);
    BIND_UNARY_NODE(Cos, heza::core::Cos);
    BIND_UNARY_NODE(Tan, heza::core::Tan);
    BIND_UNARY_NODE(Sec, heza::core::Sec);
    BIND_UNARY_NODE(Csc, heza::core::Csc);
    BIND_UNARY_NODE(Cot, heza::core::Cot);
    BIND_UNARY_NODE(Arcsin, heza::core::Arcsin);
    BIND_UNARY_NODE(Arccos, heza::core::Arccos);
    BIND_UNARY_NODE(Arctan, heza::core::Arctan);
    BIND_UNARY_NODE(Arcsec, heza::core::Arcsec);
    BIND_UNARY_NODE(Arccsc, heza::core::Arccsc);
    BIND_UNARY_NODE(Arccot, heza::core::Arccot);
    
    BIND_UNARY_NODE(Sinh, heza::core::Sinh);
    BIND_UNARY_NODE(Cosh, heza::core::Cosh);
    BIND_UNARY_NODE(Tanh, heza::core::Tanh);
    BIND_UNARY_NODE(Sech, heza::core::Sech);
    BIND_UNARY_NODE(Csch, heza::core::Csch);
    BIND_UNARY_NODE(Coth, heza::core::Coth);
    BIND_UNARY_NODE(Arcsinh, heza::core::Arcsinh);
    BIND_UNARY_NODE(Arccosh, heza::core::Arccosh);
    BIND_UNARY_NODE(Arctanh, heza::core::Arctanh);
    BIND_UNARY_NODE(Arcsech, heza::core::Arcsech);
    BIND_UNARY_NODE(Arccsch, heza::core::Arccsch);
    BIND_UNARY_NODE(Arccoth, heza::core::Arccoth);

    BIND_UNARY_NODE(Exp, heza::core::Exp);
    BIND_UNARY_NODE(Log, heza::core::Log);
    BIND_UNARY_NODE(Abs, heza::core::Abs);
    BIND_UNARY_NODE(Sign, heza::core::Sign);
    BIND_UNARY_NODE(Sqrt, heza::core::Sqrt);

    // ==========================================
    // NODOS BINARIOS ESPECIALES
    // ==========================================
    py::class_<heza::core::Root, heza::core::Expr, std::shared_ptr<heza::core::Root>>(m, "Root", "Representa una raíz de índice arbitrario (ej. raíz n-ésima).")
        .def(py::init<const heza::core::ExprPtr&, const heza::core::ExprPtr&>(), py::arg("index"), py::arg("arg"), 
            "Inicializa la raíz con su índice y el radicando.");

    py::class_<heza::core::LogBase, heza::core::Expr, std::shared_ptr<heza::core::LogBase>>(m, "LogBase", "Representa un logaritmo con base arbitraria.")
        .def(py::init<const heza::core::ExprPtr&, const heza::core::ExprPtr&>(), py::arg("base"), py::arg("arg"), 
            "Inicializa el logaritmo con su base y su argumento.");

    // ==========================================
    // UTILIDADES Y CONSTANTES GLOBALES
    // ==========================================
    m.def("make_number", &heza::core::make_number, py::arg("value"), "Crea y retorna un nodo constante numérico.");
    m.def("make_symbol", &heza::core::make_variable, py::arg("name"), "Crea y retorna un nodo de variable simbólica.");
    
    m.attr("pi") = heza::core::make_pi();
    m.attr("e") = heza::core::make_e();
    m.attr("inf") = heza::core::make_inf();
    m.attr("i") = heza::core::make_i();
}

/*
Lista completa de Clases en C++

heza::core::Expr:
    heza::core::ExprPtr eval
    heza::core::ExprPtr diff
    heza::core::ExprPtr integrate
    heza::core::ExprPtr simplify
    heza::core::ExprPtr subs
    bool is_equal
    int compare
    hash
    to_str
    to_latex
    heza::core::ExprPtr clone
    bool is_atomic
    void accept

Visitor:
    visit(Integer)
    visit(Symbol)
    ...

Number - heza::core::Expr:
    Integer
    Rational
    Real
    Complex

Constant - Number

Symbol - heza::core::Expr
Boolean - heza::core::Expr
String - heza::core::Expr

N-Arias:
Add
Mul

Binarias
Pow
Log

Unarias
Exponential

Trigonometricas

Derivative
Integral
Limit
Sum
Product

Predicate:
    Equal
    NotEqual
    Less
    LessEqual
    Greater
    GreaterEqual
    Solve 

LogicalConnective
And
Or
Xor
Not

Quatifier
Forall
Exists

Set
FiniteSet
Interval
ConditionSet
ImageSet

Tuple
List

Vector
Matrix

InductionProof
    Predicate
    Symbol
    base_case
    step_condition

Assumptions

*/