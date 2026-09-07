class Indeterminate:

    """
    Clase Indeterminate de Heza
    Es el resultado de operaciones que no tienen solucion
    """

    #Metodos principales
    def __init__(self): 
        pass

    def __str__(self) -> str:

        """
        Comportamiento al convertir a Str

        Returns:
            str: Simbolo de no existe
        """

        return "∄"
    
    def __repr__(self):

        """
        Comportamiento al convertir al representar

        Returns:
            str: Simbolo de no existe
        """

        return "∄"

    #Comparaciones
    def __eq__(self, _) -> bool:

        """
        Comportamiento al comparar

        Returns:
            bool: Siempre False, ∄ no es igual a nada, ni a si mismo
        """

        return False
    
    def __ne__(self, _) -> bool:

        """
        Comportamiento al comparar

        Returns:
            bool: Siempre True, ∄ es distinto a todo (incluyendose)
        """

        return True
    
    def __lt__(self, _):

        """
        Comportamiento al evaluar

        Raises:
            TypeError: El objeto ∄ no soporta comparaciones
        """

        raise TypeError("El obbjeto ∄ no soporta el operador <")
    
    def __le__(self, _):

        """
        Comportamiento al evaluar

        Raises:
            TypeError: El objeto ∄ no soporta comparaciones
        """
        raise TypeError("El obbjeto ∄ no soporta el operador <=")
    
    def __gt__(self, _):

        """
        Comportamiento al evaluar

        Raises:
            TypeError: El objeto ∄ no soporta comparaciones
        """
        raise TypeError("El obbjeto ∄ no soporta el operador >")
    
    def __ge__(self, _):

        """
        Comportamiento al evaluar

        Raises:
            TypeError: El objeto ∄ no soporta comparaciones
        """
        raise TypeError("El obbjeto ∄ no soporta el operador >=")
    
    #Unarios
    def __pos__(self):

        """
        Comportamiento de +∄ = ∄

        Returns:
            Indeterminate: Principio de propagacion
        """

        return self
    
    def __neg__(self):

        """
        Comportamiento de -∄ = ∄

        Returns:
            Indeterminate: Principio de propagacion
        """

        return self
    
    #Aritmeticos
    def __add__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """

        return self
    
    def __radd__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """

        return self
    
    def __sub__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """

        return self
    
    def __rsub__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __mul__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __rmul__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __truediv__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __rtruediv__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __mod__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __rmod__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __pow__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def __rpow__(self, _):

        """
        Comportammiento en operaciones aritmeticas

        Returns:
            Indeterminate: Principio de propagacion
        """
        
        return self
    
    def print(self, _):
        return "∄"
    
    def to_bool(self):

        raise TypeError("El objeto ∄ no puede convertirse a booleano")
    
    def to_number(self):

        raise TypeError("El objeto ∄ no puede convertirse a numero")
    
    def to_text(self):
        
        from .text import Text
        return Text("∄")
    
    def to_expresion(self):

        from .expresion import Expresion
        return Expresion("∄")
    
    def to_set(self):

        from .set import HezaSet as Set
        return Set([self])
    
    def to_infinite(self):

        raise TypeError("El objeto ∄ no puede convertirse a infinito")
    
    def to_null(self):

        from .null import Null
        return Null()
    
    def to_indeterminate(self):
        return self
    
    def to_abs(self):
        raise TypeError("El objeto ∄ no puede convertirse a valor absoluto")