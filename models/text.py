class Text:

    """
    Clase Text de Heza

    Cadena de caracteres

    Attributes:
        value (str): Cadena de texto

    Raises:
        ValueError: Parametro value invalido
    """

    def __init__(self, value):

        if not isinstance(value, str):
            raise ValueError("Parametro value invalido para construir Text")

        self.value = value

    def __repr__(self):
        return self.value

    def to_bool(self):

        """
        Convierte el objeto Text a Bool
        
        devuelve true el si valor de Text no es cadena vacia
        """

        from .bool import Bool

        return Bool(self.value != "")
    
    def to_number(self):
        from .number import Number

        try:
            value = float(self.value)
        except:
            raise ValueError("El objeto Text no se puede convertir a Number")
        
        return Number(value)
        
    def to_text(self):
        return self

    def to_expresion(self):
        from .expresion import Expresion
        return Expresion(self.value)
    
    def to_set(self):
        from .set import HezaSet
        return HezaSet([self])
    
    def to_infinite(self):
        raise ValueError("El objeto Text no se puede convertir a Infinite porque no se puede seleccionar el signo")

    def to_null(self):
        from .null import Null
        return Null()

    def to_indeterminate(self):
        from .indeterminate import Indeterminate
        return Indeterminate()

    def to_abs(self):

        from .number import Number

        return Number(len(self.value))

    def print(self, scope):
        return f'"{self.value}"' if scope.mode == "CONSOLE" else self.value

    def __add__(self, value):

        try:
            value = value.to_text()
        except:
            raise TypeError("El objeto Text no soporta +")
        
        return Text(self.value + value.value)
    
    def __radd__(self, value):

        try:
            value = value.to_text()
        except:
            raise TypeError("El objeto Text no soporta +")
        
        return Text(value.value + self.value)

    def __sub__(self, _):
        raise TypeError("El objeto Text no soporta el operador -")

    def __rsub__(self, _):
        raise TypeError("El objeto Text no soporta el operador -")

    def __mul__(self, other):

        from .number import Number

        if isinstance(other, Number):
            return Text(self.value * other.value)

        raise TypeError("El objeto Text no soporta el operador *")

    def __rmul__(self, _):
        raise TypeError("El objeto Text no soporta el operador *")

    def __truediv__(self, _):
        raise TypeError("El objeto Text no soporta el operador /")

    def __rtruediv__(self, _):
        raise TypeError("El objeto Text no soporta el operador /")

    def __eq__(self, value):
        
        if not isinstance(value, Text):
            raise ValueError("El objeto Text solo se puede comparar con objetos del mismo tipo")
        return value.value == self.value