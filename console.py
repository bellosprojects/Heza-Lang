"""
Archivo para obtener valores calculados con Heza desde la consola de Python.
No almacena variables, ni funciones.
Tampoco ejecuta código, solo devuelve valores calculados.
"""

from lexer import Lexer
from parser import Parser
from utils.evaluator import evaluate_node_from_scope
from models.scope import Scope
from models.number import Number
import sys
import math


def modo_calculadora_de_expresiones():
    # Declaramos un scope, solo porque el evaluador necesita un scope para evaluar, pero no se va a usar.
    scope = Scope()
    scope.mode = "CONSOLE"
    scope.add_var("pi", Number(math.pi))
    scope.add_var("e", Number(math.e))

    def evaluar_operacion(promt: str):

        promt_tokens = Lexer(code=promt).tokenize()
        promt_ast = Parser(tokens=promt_tokens).parse_expresion()
        expr_value = evaluate_node_from_scope(node=promt_ast, scope=scope)
        return expr_value.print(scope)

    prev_value = None
    # Mensaje inicial indicando que esta listo
    print("Heza Console - Listo para recibir expresiones. Escriba 'exit' para salir. ~RESET KEY~ para reiniciar ANS", flush=True)

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue

        if line.lower() == 'exit':
            sys.exit()
        elif line == '~RESET KEY~':
            prev_value = None
            print("", flush=True)
            continue

        if prev_value:
            line = line.replace('$', prev_value)

        try:
            resultado = evaluar_operacion(line)
            prev_value = resultado
            print(resultado, flush=True)
        except Exception as e:
            print(f"[Error Desconocido]: {str(e)}", flush=True)