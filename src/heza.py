from parser import Parser, ParserError
from lexer import Lexer, LexerError
from interpreter import Interpreter, InterpreterError
import sys
from console import modo_calculadora_de_expresiones
from engine import HSCE as cas
#Codigo fuente

def main():

    print(f"HEZA LANG v1.1.{cas.Sin(cas.pi).simplify().print()}")

    if len(sys.argv) > 1:

        path = sys.argv[1]

        if path == "-console":
            modo_calculadora_de_expresiones()
            return

        else:
            try:

                code = open(path, encoding="UTF-8").read()

                lexer = Lexer(code)
                tokens = lexer.tokenize()
                #print(tokens)

                parser = Parser(tokens)
                ast = parser.parse_program()
                #print(json.dumps(ast,indent=2))

                heza = Interpreter(ast)
                heza.run()
                #heza.debug()
            except FileNotFoundError:
                print("Archivo no encontrado")
                sys.exit()
            except (LexerError, ParserError, InterpreterError) as e:
                print(e)
                sys.exit()

    else:
        msg = """
Bienvenido a Heza

Para ejecutar tu archivo heza escribe en la terminal:
heza.exe tu_archivo.hz

o ejecuta tu archivo con el intérprete.

Bello's Projects
"""
        print(msg)
        input()
        sys.exit()

if __name__ == "__main__":
    main()

#https://github.com/bellosprojects/Heza/blob/main/HezaSetup.exe