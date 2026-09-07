from .error import TypeError

class Null:

    """
    Clase Null de Heza
    Representa un valor nulo o la ausencia de valor
    """

    #Metodos principales
    def __init__(self): 
        pass

    def __str__(self) -> str:

        """
        Comportamiento al convertir a str

        Returns:
            str: Simbolo de conjunto vacio
        """

        return "∅"
    
    def __repr__(self) -> str:

        """
        Comportamiento al representar

        Returns:
            str: Simbolo de conjunto vacio
        """

        return "∅"
    
    def to_bool(self):

        from .bool import Bool

        return Bool(False)
    
    def to_number(self):

        from .number import Number

        return Number(0)
    
    def to_text(self):

        from .text import Text

        return Text(self.__str__())
    
    def to_expresion(self):

        from .expresion import Expresion

        return Expresion("0")
    
    def to_set(self):

        from .set import HezaSet as Set

        return Set([self])
    
    def to_infinite(self):

        """
        Convierte el valor Null a Infinite
        Returns:
            Infinite: El valor infinito
        """

        from .infinite import Infinite

        return Infinite()
    
    def to_null(self):
        return self
    
    def to_indeterminate(self):
        from .indeterminate import Indeterminate

        return Indeterminate()
    
    def to_abs(self):
        
        from .number import Number
        return Number(0)

    def print(self, _):
        return self.__str__()

    #Comparaciones
    def __eq__(self, value) -> bool:

        """
        Compara con otros elementos

        Args:
            value: El otro valor a comparar

        Returns:
            bool: True si el otro elemento es de tipo null
        
        """

        return isinstance(value, Null)
    
    def __ne__(self, value) -> bool:

        """
        Compara con otros elementos

        Args:
            value: El otro valor a comparar

        Returns:
            bool: False si el otro elemento es de tipo null
        
        """

        return not isinstance(value, Null)
    
    def __lt__(self, value):

        """
        Aplica el operador Null < value

        Args:
            value: Otro valor a comparar

        Returns:
            bool: False si el otro elemento es de tipo Null

        Raises:
            TypeError: Un objeto Null solo se puede comparar con sigo mismo
        """

        if isinstance(value, Null):
            return False
        raise TypeError("El objeto Null no soporta el operador <")
    
    def __le__(self, value):

        """
        Aplica el operador Null <= value

        Args:
            value: Otro valor a comparar

        Returns:
            bool: True si el otro elemento es de tipo Null

        Raises:
            TypeError: Un objeto Null solo se puede comparar con sigo mismo
        """

        if isinstance(value, Null):
            return True
        raise TypeError("El objeto Null no soporta el operador <=")
    
    def __gt__(self, value):

        """
        Aplica el operador Null > value

        Args:
            value: Otro valor a comparar

        Returns:
            bool: False si el otro elemento es de tipo Null

        Raises:
            TypeError: Un objeto Null solo se puede comparar con sigo mismo
        """

        if isinstance(value, Null):
            return False
        raise TypeError("El objeto Null no soporta el operador >")
    
    def __ge__(self, value):

        """
        Aplica el operador Null >= value

        Args:
            value: Otro valor a comparar

        Returns:
            bool: True si el otro elemento es de tipo Null

        Raises:
            TypeError: Un objeto Null solo se puede comparar con sigo mismo
        """

        if isinstance(value, Null):
            return True
        raise TypeError("El objeto Null no soporta el operador >=")
    
    #Unarios
    def __pos__(self):

        """
        Operacion +Null

        Returns:
            Null: mismo valor
        """

        return self
    
    def __neg__(self):

        """
        Operacion -Null

        Returns:
            Null: mismo valor
        """
         
        return self
    
    #Aritmeticos
    def __add__(self, value):

        """
        Operacion Null + value

        Args:
            value: otro valor de la operacion

        Returns:
            value: el mismo objeto
        """

        return value
    
    def __radd__(self, value):

        """
        Operacion value + Null

        Args:
            value: otro valor de la operacion

        Returns:
            value: el mismo objeto
        """

        return value
    
    def __sub__(self, value):

        """
        Operacion Null - value

        Args:
            value: otro valor de la operacion

        Returns:
            -value: negacion aritmetica del mismo objeto
        """

        return -value
    
    def __rsub__(self, value):

        """
        Operacion Value - Null

        Args:
            value: otro valor de la operacion

        Returns:
            value: el mismo objeto
        """

        return value
    
    def __mul__(self, value):

        """
        Operacion Null * value

        Args:
            value: otro valor de la operacion

        Returns:
            Indeterminate: Si value es de tipo Infinite
            Null: En cualquier otro caso
        """

        from .infinite import Infinite

        if isinstance(value, Infinite):

            from models.indeterminate import Indeterminate

            return Indeterminate()
        return self
    
    def __rmul__(self, value):

        """
        Operacion value * Null

        Args:
            value: otro valor de la operacion

        Returns:
            Indeterminate: Si value es de tipo Infinite
            Null: En cualquier otro caso
        """

        return self.__mul__(value)
    
    def __truediv__(self, value):

        """
        Operacion Null / value

        Args:
            value: otro valor de la operacion

        Returns:
            Indeterminate: Si value es de tipo Null
            Null: En cualquier otro caso
        """

        if isinstance(value, Null):
            from models.indeterminate import Indeterminate

            return Indeterminate()
        return self
    
    def __rtruediv__(self, value):

        """
        Operacion value / Null

        Args:
            value: otro valor de la operacion

        Returns:
            Indeterminate: Si value es de tipo Null

        Raises:
            ZeroDivisionError: ∅ no puede ser un denominador
        """

        if isinstance(value, Null):
            from models.indeterminate import Indeterminate

            return Indeterminate()

        raise ZeroDivisionError("No se puede usar ∅ como denominador")
    
    def __mod__(self, value):

        """
        Operacion Null % value

        Args:
            value: Otro valor de la operacion

        Returns:
            Indeterminate: si value es de tipo Null
            Null: en otro caso
        """

        if isinstance(value, Null):
            from models.indeterminate import Indeterminate

            return Indeterminate()
        return self
    
    def __rmod__(self, _):

        """
        Operacion value % Null

        Args:
            value: otro valor a comparar

        Returns:
            Null
        """

        return self
    
    def __pow__(self, _):

        """
        Operacion Null ^ value

        Args:
            value: exponente

        Returns:
            Null
        """

        return self
    
    def __rpow__(self, value):

        """
        Operacion value ^ Null

        Returns:
            NotImplemented: Si value es de tipo Infinite

            int(1): En cualquier otro caso
        """

        from models.infinite import Infinite

        if isinstance(value, Infinite):
            return NotImplemented
        
        from .number import Number

        return Number(1)