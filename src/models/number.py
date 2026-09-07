class Number:

    """
    Clase Number de Heza

    Almacena numeros enteros y decimales

    Attributes:
        value (int|float): El numero

    Raises:
        TypeError: Valor invalido
    """

    def __init__(self, value):

        if not isinstance(value, (int, float)):
            raise ValueError("Valor invalido para construir Number")

        self.value = value

    def __repr__(self):
        return str(self.value)
    
    def __float__(self):
        return float(self.value)
    
    def __int__(self):
        return int(self.value)

    def to_number(self):
        return self

    def to_text(self):

        from .text import Text

        return Text(str(self.value))
    
    def to_bool(self):

        from .bool import Bool

        value = True if self.value != 0 and self.value != 0.0 else False
        return Bool(value)
    
    def to_expresion(self):

        from .expresion import Expresion

        return Expresion(str(self.value))
    
    def to_set(self):
        from .set import HezaSet

        return HezaSet([self])
    
    def to_null(self):
        from .null import Null
        return Null()
    
    def to_infinite(self):
        from .infinite import Infinite
        return Infinite() if self.value > 0 else Infinite(-1)
    
    def to_indeterminate(self):
        from .indeterminate import Indeterminate
        return Indeterminate()

    def to_abs(self):
        return Number(abs(self.value))

    def print(self, _):
        return str(self.value)

    def __mul__(self, other):

        if isinstance(other, Number):
            return Number(self.value * other.value)

        return NotImplemented
    
    def __mod__(self, other):
        
        if isinstance(other, Number):
            return Number(self.value % other.value)

        return NotImplemented

    def __rmod__(self, other):
        
        if isinstance(other, Number):
            return Number(other.value % self.value)

        return NotImplemented

    def __pow__(self, other):
        if isinstance(other, Number):
            return Number(self.value ** other.value)

    def __add__(self, other):

        if isinstance(other, Number):
            return Number(self.value + other.value)
        if isinstance(other, (int,float)):
            return Number(self.value + other)

        return NotImplemented

    def __sub__(self, other):

        if isinstance(other, Number):
            return Number(self.value - other.value)
        if isinstance(other, (int,float)):
            return Number(self.value - other)

        return NotImplemented
    
    def __truediv__(self, other):

        if isinstance(other, Number):
            return Number(self.value / other.value)

        return NotImplemented

    def __eq__(self, other):

        from .bool import Bool

        if isinstance(other, Number):
            return Bool(self.value == other.value)

        return NotImplemented

    def __neg__(self):
        return Number(-self.value)

    def __gt__(self, value):
        
        return self.value > value.value

    def __ge__(self, value):
        return self.value >= value.value
    
    def __hash__(self):
        return hash(self.value)