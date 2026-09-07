from .bool import Bool
from .number import Number
from .text import Text
from .null import Null
from .infinite import Infinite
from .indeterminate import Indeterminate

class HezaObject:
    def __init__(self, attributes=None, name=None):
        # attributes es un diccionario {nombre_atributo: valor}
        self.attributes = attributes if attributes is not None else {}
        self.name = name  # opcional, solo para depuración

    def get(self, attr_name):
        if attr_name in self.attributes:
            return self.attributes[attr_name]
        raise AttributeError(f"El objeto '{self.name or ''}' no tiene el atributo '{attr_name}'")

    def set(self, attr_name, value):
        self.attributes[attr_name] = value

    def to_bool(self):
        # Un objeto es verdadero si tiene al menos un atributo (o siempre True?)
        # Decidimos que un objeto siempre es True, incluso vacío (coherente con otros lenguajes)
        return Bool(True)

    def to_number(self):
        raise ValueError("No se puede convertir un objeto a Number")

    def to_text(self):
        # Representación textual: {atr1: val1, atr2: val2, ...}
        parts = [f"{k}: {v}" for k, v in self.attributes.items()]
        return Text("{" + ", ".join(parts) + "}")

    def to_expresion(self):
        raise ValueError("No se puede convertir un objeto a Expresion")

    def to_set(self):
        # Podríamos convertir los atributos a un conjunto de tuplas (clave, valor)
        # Pero no es necesario, mejor lanzar error
        raise ValueError("No se puede convertir un objeto a Set")

    def to_infinite(self):
        return Infinite()

    def to_null(self):
        return Null()

    def to_indeterminate(self):
        return Indeterminate()

    def to_abs(self):
        return Number(len(self.attributes))

    def __eq__(self, other):
        if not isinstance(other, HezaObject):
            return False
        # Comparar todos los atributos (claves y valores)
        if self.attributes.keys() != other.attributes.keys():
            return False
        for key in self.attributes:
            if self.attributes[key] != other.attributes[key]:
                return False
        return True

    def __ne__(self, other):
        return not self.__eq__(other)

    def __repr__(self):
        return f"Object({self.attributes})"

    def print(self, scope):
        # Para el método print en conjuntos, podemos reutilizar to_text
        return self.to_text().value