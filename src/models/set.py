EMPYSET = set()

class HezaSet:

    def __init__(self, values: set):
        self.values = set(values)
    
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

        result = []
        for a in self.values:
            for b in other.values:
                nuevos_elementos = []
                if isinstance(a, HezaTuple):
                    nuevos_elementos += a.values
                else:
                    nuevos_elementos += [a]
                if isinstance(b, HezaTuple):
                    nuevos_elementos += b.values
                else:
                    nuevos_elementos += [b]
                result.append(HezaTuple(nuevos_elementos))

        return HezaSet(set(result))
        
    def __sub__(self, other):
        if not isinstance(other, HezaSet):
            raise TypeError(f"No se puede restar con {type(other)}")
        return HezaSet(self.values - other.values)

    def __pow__(self, other):
        from .number import Number
        if not isinstance(other, Number):
            raise TypeError("Un objeto Set solo se puede elevar a un numero finito")

        exp = int(other.value)

        if exp < 0:
            raise ValueError("El objeto Set solo se puede elevar a un numero entero mayor o igual a 0")

        if exp == 0:
            return HezaSet(set())

        initial = HezaSet(self.values.copy()) 
        result = initial

        for _ in range(exp - 1):

            result = result.cross(initial)
            

        return result

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
        if len(self.values) != len(other.values):
            return False
        return self.values == other.values
    
    def eliminar(self, value):

        new_set = {v for v in self.values if v != value}

        return HezaSet(new_set)
    
    def __hash__(self):
        return hash(tuple(self.values))