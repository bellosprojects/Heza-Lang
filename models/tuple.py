from .number import Number
from .bool import Bool
from .text import Text
from .null import Null
from .infinite import Infinite
from .indeterminate import Indeterminate

class HezaTuple:
    def __init__(self, values):
        self.values = values

    def get(self, index):
        if not isinstance(index, Number):
            raise ValueError("El índice debe ser un número")
        idx = int(index.value)
        if idx < 0 or idx >= len(self.values):
            raise IndexError("Índice fuera de rango")
        return self.values[idx]

    def to_abs(self):
        return Number(len(self.values))

    def to_bool(self):
        return Bool(len(self.values) > 0)

    def to_number(self):
        return Number(len(self.values))

    def to_text(self):
        from .text import Text
        return Text("(" + ", ".join(str(v) for v in self.values) + ")")

    def to_set(self):
        from .set import HezaSet
        return HezaSet(self.values.copy())

    def to_infinite(self):
        return Infinite()

    def to_null(self):
        return Null()

    def to_indeterminate(self):
        return Indeterminate()

    def __repr__(self):
        return f"({self.values})"
    
    def print(self, scope):
        return self.to_text().value
    
    def __eq__(self, other):
        if not isinstance(other, HezaTuple):
            return False
        if len(self.values) != len(other.values):
            return False
        return all(a == b for a, b in zip(self.values, other.values))
    
    def __hash__(self):
        return hash(tuple(self.values))