class HezaSet:

    def __init__(self, values: set):
        self.values = set(values)

    def get(self, index):

        from models import Number

        if not isinstance(index, Number):
            raise ValueError("EL indice debe ser un objeto de tipo Number")

        idx = int(index.value)

        if idx < 0 or idx > len(self.values):
            raise IndexError("Indice fuea de rango")
        
        return list(self.values)[idx]
    
    def add(self, new_value):
        if new_value is self:
            
            self.values.add(HezaSet(self.values))
        else:
            self.values.add(new_value)

    def union(self, other):
        if not isinstance(other, HezaSet):
            raise TypeError(f"No se puede unir con {type(other)}")
        return HezaSet(self.values | other.values)

    def inter(self, other):
        if not isinstance(other, HezaSet):
            raise TypeError(f"No se puede intersecar con {type(other)}")
        return HezaSet(self.values & other.values)

    def symetric_difference(self, other):
        if not isinstance(other, HezaSet):
            raise TypeError(f"No se puede hacer diferencia simétrica con {type(other)}")
        return HezaSet(self.values ^ other.values)

    def cross(self, other):
        if not isinstance(other, HezaSet):
            raise TypeError(f"No se puede hacer producto cartesiano con {type(other)}")
        from .tuple import HezaTuple
        return HezaSet({HezaTuple([a, b]) for a in self.values for b in other.values})
        
    def __sub__(self, other):
        if not isinstance(other, HezaSet):
            raise TypeError(f"No se puede restar con {type(other)}")
        return HezaSet(self.values - other.values)

    def print(self, scope):
        if len(self.values) == 0:
            return "{}"
        return "{" + ", ".join([str(value.print(scope)) for value in self.values]) + "}"

    def to_bool(self):
        
        from .bool import Bool

        return Bool(len(self.values) > 0)
    
    def to_number(self):

        from .number import Number
        return Number(len(self.values))
    
    def to_text(self):

        from .text import Text
        return Text(self.print(None))
    
    def to_expresion(self):

        raise ValueError("No se puede convertir un Set a Expresion")
    
    def to_set(self):
        return self
    
    def to_infinite(self):

        from .infinite import Infinite
        return Infinite()
    
    def to_null(self):
        from .null import Null
        return Null()
    
    def to_indeterminate(self):

        from .indeterminate import Indeterminate
        return Indeterminate()

    def to_abs(self):
        from .number import Number
        return Number(len(self.values))

    def __repr__(self):
        return f"Set({self.values})"
    
    def __eq__(self, other):
        if not isinstance(other, HezaSet):
            return False
        # Comparar los valores de ambos conjuntos (orden importa si son listas)
        # Si quieres que sean conjuntos sin orden, deberías usar sets, pero como es una lista,
        # comparamos elemento por elemento en orden.
        if len(self.values) != len(other.values):
            return False
        return self.values == other.values
    
    def eliminar(self, value):

        new_set = {v for v in self.values if v != value}

        return HezaSet(new_set)
    
    def __hash__(self):
        return None