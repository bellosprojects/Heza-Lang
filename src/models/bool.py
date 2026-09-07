class Bool:

    """
    Clase Bool de Heza

    false o true

    Attributes:
        value (bool):

    Raises:
        ValueError: Parametro value invalido
    """

    def __init__(self, value):
        
        if isinstance(value, Bool):
            self.value = value.value

        elif isinstance(value, bool):
            self.value = value
        else:
            raise ValueError("Parametro value invalido para construir Bool")

    def __repr__(self):
        return "true" if self.value else "false"

    def __bool__(self):
        return self.value
    
    def to_bool(self):
        return self
    
    def to_number(self):
        from models import Number
        return Number(1 if self.value else 0)
    
    def to_text(self):
        from models import Text
        return Text("true" if self.value else "false")
    
    def to_expresion(self):
        from models import Expresion
        return Expresion("1" if self.value else "0")
    
    def to_set(self):
        from models import HezaSet as Set
        return Set([self])
    
    def to_infinite(self):
        from models import Infinite
        return Infinite(1 if self.value else -1)
    
    def to_null(self):
        from models import Null
        return Null()
    
    def to_indeterminate(self):
        from models import Indeterminate
        return Indeterminate()
    
    def to_abs(self):
        return self.to_number()
    
    def print(self, _):
        return self.__repr__()