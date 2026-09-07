from abc import ABC, abstractmethod
from .number import Number
from .bool import Bool
from .infinite import Infinite
from .tuple import HezaTuple

class HezaSet(ABC):

    """
    Clase Generica Para Conjuntos del Lenguaje Heza

    Cross aun no se implementa porque requiere crea un CompressionSet (No implementado aun)
    """

    def __init__(self):
        self.amplitude = None
        self.dimension  = 1

    @abstractmethod
    def contains(self, value) -> Bool: ...

    @abstractmethod
    def get_iterator(self) -> list: ...

    @abstractmethod
    def union(self, other: HezaSet) -> HezaSet: ... # type: ignore

    @abstractmethod
    def intersection(self, other: HezaSet) -> HezaSet: ... # type: ignore

    @abstractmethod
    def symetric_difference(self, other: HezaSet) -> HezaSet: ... # type: ignore

    @abstractmethod
    def sub(self, other: HezaSet) -> HezaSet: ... # type: ignore

    """Reales - self"""
    @abstractmethod
    def complement(self) -> HezaSet: ... # type: ignore

    @abstractmethod
    def cardinality(self) -> Number | Infinite: ...

    @abstractmethod
    def cross(self, other: HezaSet) -> HezaSet: ... # type: ignore

class SetUnion(HezaSet):
    
    """
    Union de conjuntos
    (Uno o ambos no son Finitos)
    """

    def __init__(self, left: HezaSet, right: HezaSet):
        self.left = left
        self.right = right

    def contains(self, value):
        return Bool(self.left.contains(value=value) or 
                self.right.contains(value=value))
    
    def union(self, other: HezaSet):
        return SetUnion(
            left=self,
            right=other
        )
    
    def intersection(self, other: HezaSet):
        return SetIntersection(
            left=self,
            right=other
        )
    
    def symetric_difference(self, other: HezaSet):
        return SetSymetricDifference(
            left=self,
            right=other
        )
    
    def sub(self, other: HezaSet):
        return SetSub(
            left=self,
            right=other
        )
    
    def complement(self):
        return SetSub(
            left=HezaRelasNumbers(),
            right=self
        )
    
    def cardinality(self):
        return Infinite()
    
class SetIntersection(HezaSet):

    """
    Interseccion de conjuntos
    (Uno o ambos no es Finitos)
    """

    def __init__(self, left: HezaSet, right: HezaSet):
        self.left = left
        self.right = right

    def contains(self, value):
        return Bool(self.left.contains(value=value) and 
                self.right.contains(value=value))
    
    def union(self, other: HezaSet):
        return SetUnion(
            left=self,
            right=other
        )
    
    def intersection(self, other: HezaSet):
        return SetIntersection(
            left=self,
            right=other
        )
    
    def symetric_difference(self, other: HezaSet):
        return SetSymetricDifference(
            left=self,
            right=other
        )
    
    def sub(self, other: HezaSet):
        return SetSub(
            left=self,
            right=other
        )
    
    def complement(self):
        return SetSub(
            left=HezaRelasNumbers(),
            right=self
        )
    
    def cardinality(self):
        return Infinite()
    
class SetSymetricDifference(HezaSet):

    """
    Diferencia Simetrica de conjuntos
    (Uno o ambos no es Finitos)
    """

    def __init__(self, left: HezaSet, right: HezaSet):
        self.left = left
        self.right = right

    def contains(self, value):
        return Bool((self.left.contains(value=value) and not
                self.right.contains(value=value)) or 
                (self.right.contains(value=value) and not
                self.left.contains(value=value)))
    
    def union(self, other: HezaSet):
        return SetUnion(
            left=self,
            right=other
        )
    
    def intersection(self, other: HezaSet):
        return SetIntersection(
            left=self,
            right=other
        )
    
    def symetric_difference(self, other: HezaSet):
        return SetSymetricDifference(
            left=self,
            right=other
        )
    
    def sub(self, other: HezaSet):
        return SetSub(
            left=self,
            right=other
        )
    
    def complement(self):
        return SetSub(
            left=HezaRelasNumbers(),
            right=self
        )
    
    def cardinality(self):
        return Infinite()
    
class SetSub(HezaSet):

    """
    Diferencia de conjuntos
    (Uno o ambos no es Finitos)
    """

    def __init__(self, left: HezaSet, right: HezaSet):
        self.left = left
        self.right = right

    def contains(self, value):
        return Bool(self.left.contains(value=value) and not
                self.right.contains(value=value))
    
    def union(self, other: HezaSet):
        return SetUnion(
            left=self,
            right=other
        )
    
    def intersection(self, other: HezaSet):
        return SetIntersection(
            left=self,
            right=other
        )
    
    def symetric_difference(self, other: HezaSet):
        return SetSymetricDifference(
            left=self,
            right=other
        )
    
    def sub(self, other: HezaSet):
        return SetSub(
            left=self,
            right=other
        )
    
    def complement(self):
        return SetSub(
            left=HezaRelasNumbers(),
            right=self
        )
    
    def cardinality(self):
        return Infinite()
    
class EmptySet(HezaSet):

    """
    Conjuto Vacio
    """

    def __init__(self):
        pass

    def contains(self, _):
        return Bool(False)
    
    def union(self, other: HezaSet):
        return other
    
    def intersection(self, _: HezaSet):
        return self
    
    def symetric_difference(self, other: HezaSet):
        return other
    
    def sub(self, _: HezaSet):
        return self
    
    def complement(self):
        return HezaRelasNumbers()
    
    def cardinality(self):
        return Number(0)
    
class HezaRelasNumbers(HezaSet):

    """
    ahorita la implemento
    """
    pass