class Expresion:

    """
    Clase Expresion Heza (expresion simbolica)

    Representa una expresion simbolica en formato str y usa el metodo de lazy-loading 
    para convertir la expresion de formato string a un AST de Heza y usando el Lexer/Parser
    y de alli construir un objeto sympy que se pueda usar para resolver operaciones sumbolicas

    Attributes:
        value (str): expresion simbolica
        ast (dict | None): AST de la expresion en formato Heza
        sympy (dict | None): Objeto sympy construido con el ast
    """

    def __init__(self, value: str):
        self.value = value
        self.ast = None
        self.sympy = None

    def __str__(self) -> str:

        """
        Define como se comporta el objeto al convertir a str

        Returns:
            value (str): expresion simbolica
        """

        return self.value
    
    def __repr__(self) -> str:

        """
        Metodo que devuelve el atributo value formateado

        Returns:
            value (str): expresion simbolica formateada
        """

        return f"Expresion({self.value})"

    def generate_ast(self):

        """
        Metodo para generar el AST partiendo del atributo value
        usando las clases Lexer/Parser de Heza

        Raises:
            LexerError: token desconocido
            ParserError: token invalido para una expresion simbolica
        """

        from lexer import Lexer, LexerError
        from parser import Parser, ParserError

        try:
            tokens = Lexer(self.value).tokenize()
            self.ast = Parser(tokens=tokens).parse_expresion()

        except (ParserError, LexerError) as e:
            raise e
        
    def generate_sympy(self):

        """
        Metodo que genera el objeto sympy partiendo del AST

        Verifica que el AST este creado y luego usa la funcion heza_ast_to_sympy
        del paquete utils para generar el objeto sympy
        """

        if self.ast is None:
            self.generate_ast()

        from utils import heza_ast_to_sympy
        
        self.sympy = heza_ast_to_sympy(self.ast)

    def create_heza_ast(self, value):
        from lexer import Lexer, LexerError
        from parser import Parser, ParserError

        try:
            tokens = Lexer(value).tokenize()
            return Parser(tokens=tokens).parse_expresion()

        except (ParserError, LexerError) as e:
            raise e

    def evaluate(self, var:str, value):
        """
        Sustituye la variable 'var' por 'value' en la expresión simbólica.
        
        Args:
            var (str): Variable a sustituir en la expresion
            value (dict): AST de Heza para convertir en objeto sympy y sustituir por var

        Returns:
            Expresion: nuevo objeto Expresion con la sustitucion

        """
        import sympy as sp
        from utils import heza_ast_to_sympy
        from utils.sympy_generator import sympy_to_heza

        if isinstance(value, Expresion):
            value_sympy = heza_ast_to_sympy(self.create_heza_ast(value.value))
        else:
            value_sympy = float(value.to_number())

        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()

        symbol_to_sub = sp.Symbol(var)
        
        result_sympy = self.sympy.subs(symbol_to_sub, value_sympy)
        
        new_value_str = str(result_sympy)
        
        return Expresion(sympy_to_heza(new_value_str))
    
    def derive(self, var: str):
        """
        Calcula la derivada de la expresión con respecto a 'var'.
        
        Args:
            var (str): Variable respecto a la cual se va a derivar

        Returns:
            Expresion: nueva expresion simbolica (derivada de la original)
        """
        import sympy as sp
        from utils.sympy_generator import sympy_to_heza
        
        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()

        symbol_to_derive = sp.Symbol(var)
        
        result_sympy = self.sympy.diff(symbol_to_derive)
        
        new_value_str = str(result_sympy)
        
        return Expresion(sympy_to_heza(new_value_str))
    
    def integrate(self, var: str):
        """
        Calcula la integral indefinida de la expresión con respecto a 'var'.
        
        Args:
            var (str): Nombre de la variable respecto a la cual se va a integrar

        Returns:
            Expresion: Nueva expresion simbolica (la integral)
        """
        import sympy as sp
        from utils.sympy_generator import sympy_to_heza
        
        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()

        symbol_to_integrate = sp.Symbol(var)
        
        result_sympy = sp.integrate(self.sympy, symbol_to_integrate)
        
        new_value_str = str(result_sympy)
        
        return Expresion(sympy_to_heza(new_value_str))
    
    def integrate_definite(self, var: str, lower_limit, upper_limit):
        """
        Calcula la integral definida de la expresión entre los límites [a, b]
        con respecto a `var`.
        
        Args:
            var (str): Nombre de la variable respecto a la cual se integra.
            lower_limit (int|float): Limite inferior de la integral.
            upper_limit (int|float): Limite superior de la integral.

        Returns:
            Expresion: Resultado del calculo en formato Expresion

        Raises:
            ValueError: limite inferior o supeior en formato invalido.
        """
        import sympy as sp
        from utils.sympy_generator import sympy_to_heza, heza_ast_to_sympy

        if self.sympy is None:
            if self.ast is None:
                    self.generate_ast()
            self.generate_sympy()

        symbol_to_integrate = sp.Symbol(var)

        if isinstance(lower_limit, Expresion):
            lower_limit.generate_ast()
            lower_limit = heza_ast_to_sympy(lower_limit.ast)

        limits = (symbol_to_integrate, lower_limit, upper_limit)

        result_sympy = sp.integrate(self.sympy, limits)

        new_value_str = str(result_sympy)

        return Expresion(sympy_to_heza(new_value_str))
    
    def limit(self, var: str, point, direction=None):
        """
        Calcula el límite de la expresión cuando 'var' tiende a 'point'.
        
        Args:
            var (str): Nombre de la variable a usar para resolver el limite.
            point (dict): valor al cual tiende la variable en formato AST de Heza que se convertira en un objeto sympy
            direction (str|None): Indica si la variable tiende a value por la derecha (+), por la izquierda (-) o no es unilateral (None)

        Returns:
            Expresion: Resultado del limite en formato Expresion
        """
        import sympy as sp
        from utils import heza_ast_to_sympy, sympy_to_heza
        
        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()

        sympy_point = heza_ast_to_sympy(point)

        symbol = sp.Symbol(var)
        
        if direction:
            result_sympy = sp.limit(self.sympy, symbol, sympy_point, dir=direction)
        else:
            result_sympy = sp.limit(self.sympy, symbol, sympy_point)
        
        new_value_str = str(result_sympy)
        
        return Expresion(sympy_to_heza(new_value_str))
    
    def combine_expresion(self, other_value, operator: str, isR = False):

        """
        Metodo Auxiliar de Expresion para combinar expresiones

        Args:
            other_value: valor con el cual se va a combinar
            operator (str): +,-,*,/,^,% (aritmeticos)
            isR (bool): Indica si es reflejada o normal

        Returns:
            Expresion: Resultado de la nueva expresion combinada

        Raises:
            TypeError: other_value es de un tipo incompatible
        """

        from .number import Number
        from .text import Text

        if isinstance(other_value, (Expresion, Number, Text)):
            other_value = other_value.to_text().value
        else:
            raise TypeError(f"Expresion no se puede combinar con el tipo de dato: {type(other_value)}")

        if isR:
            new_value = f"({other_value}) {operator} ({self.value})"
        else:
            new_value = f"({self.value}) {operator} ({other_value})"

        return Expresion(new_value)
    
    def to_bool(self):
        from .bool import Bool

        """
        Convierte la expresion a booleano (si se puede)

        Returns:
            Bool: Resultado de la expresion si no contiene variables libres

        Raises:
            TypeError: La expresion aun contiene variables libres y nu se puede expresar como booleano
        """

        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()
            
        if self.sympy.is_number or not self.sympy.free_symbols:
            return Bool(bool(self.sympy.evalf()))
        else:
            raise TypeError(f"No se puede convertir la Expresion '{self.value}' a booleano: contiene variables libres.")

    def to_number(self):
        
        from .number import Number

        """
        Convierte la expresion a decimal (si se puede)

        Returns:
            float: Resultado de la expresion si no contiene variables libres

        Raises:
            TypeError: La expresion aun contiene variables libres y nu se puede expresar como numero
        """

        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()
            
        if self.sympy.is_number or not self.sympy.free_symbols:
            return Number(float(self.sympy.evalf()))
        else:
            raise TypeError(f"No se puede convertir la Expresion '{self.value}' a número: contiene variables libres.")
        
    def to_text(self):
        from .text import Text

        """
        Convierte la expresion a texto

        Returns:
            Text: Resultado de la expresion en formato texto
        """

        return Text(self.value)
    
    def to_expresion(self):
        """
        Convierte la expresion a Expresion (si no lo es ya)

        Returns:
            Expresion: La misma expresion
        """

        return self
    
    def to_set(self):
        from .set import HezaSet as Set

        """
        Convierte la expresion a conjunto

        Returns:
            Set: Resultado de la expresion en formato conjunto
        """

        return Set([self])
    
    def to_infinite(self):
        from .infinite import Infinite

        """
        Convierte la expresion a infinito

        Returns:
            Infinite: Resultado de la expresion en formato infinito
        """

        try:
            return Infinite( 1 if self.to_number().value >= 0 else -1 )
        except:
            raise TypeError(f"No se puede convertir la Expresion {self.value} a infinito: contiene variables libres.")

    def to_null(self):
        
        from .null import Null

        """
        Convierte la expresion a nulo

        Returns:
            Null: Resultado de la expresion en formato nulo
        """

        return Null()
    
    def to_indeterminate(self):
        from .indeterminate import Indeterminate

        """
        Convierte la expresion a indeterminado

        Returns:
            Indeterminate: Resultado de la expresion en formato indeterminado
        """

        return Indeterminate()
    
    def to_abs(self):

        from .number import Number

        """
        Convierte la expresion a su valor absoluto (si se puede)
        Returns:
            Number: Resultado de la expresion en formato numero (absoluto)
        Raises:
            TypeError: La expresion aun contiene variables libres y nu se puede expresar como numero
        """

        if self.sympy is None:
            if self.ast is None:
                self.generate_ast()
            self.generate_sympy()

        if self.sympy.is_number or not self.sympy.free_symbols:
            return Number(float(abs(self.sympy.evalf())))
        
        else:
            raise TypeError(f"No se puede convertir la Expresion '{self.value}' a número absoluto: contiene variables libres.")
        

    def __add__(self, value):

        """
        Combina las exrpresion con la operacion suma.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "+", False)
    
    def __radd__(self, value):
        
        """
        Combina las exrpresion con la operacion suma.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "+", True)
    
    def __sub__(self, value):
        
        """
        Combina las exrpresion con la operacion resta.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "-", False)
    
    def __rsub__(self, value):
        
        """
        Combina las exrpresion con la operacion resta reflejada.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "-", True)
    
    def __mul__(self, value):
        
        """
        Combina las exrpresion con la operacion producto.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "*", False)
    
    def __rmul__(self, value):
        
        """
        Combina las exrpresion con la operacion suma.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "*", True)
    
    def __truediv__(self, value):
        
        """
        Combina las exrpresion con la operacion cociente.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "/", False)
    
    def __rtruediv__(self, value):
        
        """
        Combina las exrpresion con la operacion cociente reflejado.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "/", True)
    
    def __mod__(self, value):

        
        """
        Combina las exrpresion con la operacion mod.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "%", False)
    
    def __rmod__(self, value):
        
        """
        Combina las exrpresion con la operacion mod reflejado.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "%", True)
    
    def __pow__(self, value):
        
        """
        Combina las exrpresion con la operacion potencia.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "^", False)
    
    def __rpow__(self, value):
        
        """
        Combina las exrpresion con la operacion potencia reflejada.

        Args:
            value: otra expresion (debe ser str,float,int o Expresion)

        Returns:
            Expresion: (operacion combinada)
        """

        return self.combine_expresion(value, "^", True)
    
    def __eq__(self, _):

        """
        Metodo para comparar con otros valores (no es posible)

        Raises:
            TypeError: Los objetos expresion no son comparables
        """

        raise TypeError("Los objetos expresion no se pueden comparar, intenta conseguir el valor numerico y luego comparar")
    
    def __ne__(self, other):

        if isinstance(other, Expresion):
            return self.value != other.value
        return True


        """
        Metodo para comparar con otros valores (no es posible)

        Raises:
            TypeError: Los objetos expresion no son comparables
        """

        raise TypeError("Los objetos expresion no se pueden comparar, intenta conseguir el valor numerico y luego comparar")
    
    def __lt__(self, _):

        """
        Metodo para comparar con otros valores (no es posible)

        Raises:
            TypeError: Los objetos expresion no son comparables
        """

        raise TypeError("Los objetos expresion no se pueden comparar, intenta conseguir el valor numerico y luego comparar")
    
    def __le__(self, _):

        """
        Metodo para comparar con otros valores (no es posible)

        Raises:
            TypeError: Los objetos expresion no son comparables
        """

        raise TypeError("Los objetos expresion no se pueden comparar, intenta conseguir el valor numerico y luego comparar")
    
    def __gt__(self, _):

        """
        Metodo para comparar con otros valores (no es posible)

        Raises:
            TypeError: Los objetos expresion no son comparables
        """

        raise TypeError("Los objetos expresion no se pueden comparar, intenta conseguir el valor numerico y luego comparar")
    
    def __ge__(self, _):

        """
        Metodo para comparar con otros valores (no es posible)

        Raises:
            TypeError: Los objetos expresion no son comparables
        """

        raise TypeError("Los objetos expresion no se pueden comparar, intenta conseguir el valor numerico y luego comparar")

    def print(self, scope):
        return f"'{str(self.value)}'" if scope.mode == "CONSOLE" else str(self.value)

    def __hash__(self):
        return hash(self.value)