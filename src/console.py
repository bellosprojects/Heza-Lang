"""
Archivo para obtener valores calculados con Heza desde la consola de Python.
No almacena variables, ni funciones.
Tampoco ejecuta código, solo devuelve valores calculados.
"""

from lexer import Lexer
from parser import Parser
from utils.evaluator import evaluate_node_from_scope
from models.scope import Scope
import engine.HSCE as cas
import math

from tokens import keyword as KeywordsRegex

import signal, os
from prompt_toolkit import PromptSession
from prompt_toolkit.lexers import PygmentsLexer
from prompt_toolkit.history import FileHistory
from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.styles import Style
from pygments.lexer import RegexLexer
from prompt_toolkit import print_formatted_text
from prompt_toolkit.formatted_text import FormattedText
from pygments.token import Text, Operator, Number, Keyword, Punctuation, Name, String, Literal, Comment


def modo_calculadora_de_expresiones():
    class HezaConsoleLexer(RegexLexer):
        name = 'Heza'
        tokens = {
            'root': [

                #palabras

                #Signos
                (r'←', Operator),
                (r'→', Operator),
                (r'∈', Operator),
                (r'∉', Operator),
                (r'\\', Operator),
                (r'∧', Operator),
                (r'∨', Operator),
                (r'¬', Operator),
                (r'√', Operator),
                (r'∅', Literal),
                (r'∀', Operator),
                (r'∃', Operator),
                (r'⊂', Operator),
                (r'≠', Operator),
                (r'∑', Operator),
                (r'∏', Operator),
                (r'∞', Literal),
                (r'∄', Literal),
                (r'∂', Operator),
                (r'∫', Operator),
                (r'∪', Operator),
                (r'⋂', Operator),
                (r'Δ', Operator),
                (r'∇', Operator),
                (r'⨉', Operator),
                (r'∙', Operator),
                
                #Patrones
                (r'\d+(\.\d+)?([eE][+-]\d+)?', Number),
                (r'"([^"]*)"', String),
                (r"'([^']*)'", Literal),

                #\t \n ESP
                (r'\s', Text),

                #Comentarios
                (r'~(.*)', Comment),

                #Operadores
                (r'==', Operator),
                (r'=', Operator),
                (r'>=', Operator),
                (r'<=', Operator),
                (r'>>', Operator),
                (r'<<', Operator),
                #('Q', r'\?'),
                #('DOUBLEEXC', r'!!'),
                (r'!', Operator),
                (r'<', Operator),
                (r'>', Operator),
                (r'\+', Operator),
                (r'-', Operator),
                (r'\*', Operator),
                (r'/', Operator),
                (r'%', Operator),
                (r'\^', Operator),
                (r'\.\.', Text),
                (r'\.', Text),
                (r',', Punctuation),
                (r'@', Literal),
                (r'&', Operator),
                (r':', Operator),
                (r'\(', Punctuation),
                (r'\)', Punctuation),
                (r'\[', Punctuation),
                (r'\]', Punctuation), 
                (r'\{', Punctuation),
                (r'\}', Punctuation),
                #('DOLLAR', r'\$'),
                (r'\|', Operator),

                (rf'\b({"|".join(KeywordsRegex + ["exit"])})\b', Keyword),

                (r'\b(true|flase)\b', Literal),

                (r'[a-zA-Z_]\w*', Name),
            ],
        }

    style = Style.from_dict({
        'prompt': 'ansicyan bold',
        'result': 'ansigreen',
        'continuation': 'ansiyellow',
        'error': 'ansired bold'
    })

    bindings = KeyBindings()

    @bindings.add('c-c')
    def _(event):
        """Ctrl+C: cancela la linea actual"""
        event.app.current_buffer.reset()

    @bindings.add('c-z')
    def _(event):
        os.kill(0, signal.SIGTERM)

    session = PromptSession(
        lexer=PygmentsLexer(HezaConsoleLexer),
        history=FileHistory('heza_history.txt'),
        key_bindings=bindings,
        style=style
    )
    os.system("CLS")

    # Declaramos un scope, solo porque el evaluador necesita un scope para evaluar, pero no se va a usar.
    print("Heza Console - Listo para recibir expresiones. \n\nEscriba 'exit' para salir. \n~RESET KEY~ para reiniciar ANS\n", flush=True)
    scope = Scope()
    scope.mode = "CONSOLE"
    scope.add_var("pi", cas.Number(math.pi))
    scope.add_var("e", cas.Number(math.e))

    def evaluar_operacion(promt: str):

        promt_tokens = Lexer(code=promt).tokenize()
        promt_ast = Parser(tokens=promt_tokens).parse_expresion()
        expr_value = evaluate_node_from_scope(node=promt_ast, scope=scope)
        return expr_value.__str__()

    ans = None
    # Mensaje inicial indicando que esta listo

    while True:
        try:
            text = session.prompt('HEZA >>> ', style=style)
        except KeyboardInterrupt:
            continue
        except EOFError:
            break

        line = text.strip()

        if line.lower() == 'exit':
            break
        elif line == '~RESET KEY~':
            ans = None
            print("", flush=True)

        else:

            try:

                if ans:
                    line = line.replace('$', ans)

                resultado = evaluar_operacion(line)
                ans = resultado

                print_formatted_text(FormattedText([('class:result', resultado)]), style=style)

            except Exception as e:
                print_formatted_text(FormattedText([('class:error', f"Error {e}")]), style=style)
