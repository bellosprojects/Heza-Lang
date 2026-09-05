from .error import TypeError

class Infinite:

    """
    Clase Infinite de Heza
    Actua como una definicion, no como un numero

    Attributes:
        sign (int): Singo del infinito (1 positivo, -1 negativo)

    Raises:
        ValueError: Valor invalido para el signo 
    """

    #Metodos principales
    def __init__(self, sign = 1): 
        if sign not in (1, -1):
            raise ValueError("El signo debe ser 1 o -1")
        self.sign = sign

    def __str__(self) -> str:

        """
        Comportamiento como str

        Returns:
            str: Simbolo del infinito con su signo
        """

        return "-∞" if self.sign == -1 else "∞"
    
    def __repr__(self) -> str:

        """
        Comportamiento como repr

        Returns:
            str: Simbolo del infinito con su signo
        """

        return self.__str__()
    
    #Comparaciones
    def __eq__(self, value) -> bool:

        """
        Verificacion de igualdad

        Args:
            value: valor con el que se realiza la comparacion

        Returns:
            bool: True si ambos sin Infinite con el mismo signo
        """

        if isinstance(value, Infinite):
            return self.sign == value.sign
        return False
    
    def __ne__(self, value) -> bool:

        """
        Verificacion de desigualdad

        Args:
            value: valor con el que se realiza la comparacion

        Returns:
            bool: Negacion de la igualdad
        """

        return not self.__eq__(value)
    
    def __lt__(self, value):

        """
        Operacion Infinite > value
        
        Args:
            value: valor con el que se realiza la comparacion

        Returns:
            bool: Si se puede realizar la comparacion
            NotImplemented: en otro caso
        """

        if isinstance(value, Infinite):
            return self.sign < value.sign
        
        if self.sign == 1:
            return False
        if self.sign == -1:
            return True
        
        return NotImplemented
    
    def __le__(self, value):

        """
        Operacion Infinite > value
        
        Args:
            value: valor con el que se realiza la comparacion

        Returns:
            bool: Si se puede realizar la comparacion
            NotImplemented: en otro caso
        """

        if isinstance(value, Infinite):
            return self.sign <= value.sign
        
        if self.sign == 1:
            return False
        if self.sign == -1:
            return True
        
        return NotImplemented
    
    def __gt__(self, value):

        """
        Operacion Infinite > value
        
        Args:
            value: valor con el que se realiza la comparacion

        Returns:
            bool: Si se puede realizar la comparacion
            NotImplemented: en otro caso
        """

        if isinstance(value, Infinite):
            return self.sign > value.sign
        
        if self.sign == 1:
            return True
        
        if self.sign == -1:
            return False
        
        return NotImplemented
    
    def __ge__(self, value):

        """
        Operacion Infinite > value
        
        Args:
            value: valor con el que se realiza la comparacion

        Returns:
            bool: Si se puede realizar la comparacion
            NotImplemented: en otro caso
        """

        if isinstance(value, Infinite):
            return self.sign >= value.sign
        
        if self.sign == 1:
            return True
        if self.sign == -1:
            return False
        
        return NotImplemented
    
    #Unarios
    def __pos__(self):

        """
        Operacion +Infinite

        Returns:
            Infinite: mismo objeto
        """

        return Infinite
    
    def __neg__(self):

        """
        Operacion -Infinite
        
        Returns:
            Infinite: infinito con el signo contrario
        """

        return Infinite(-self.sign)
    
    #Aritmeticos
    def __add__(self, value):

        """Operacion Infinite + value.

        Parameters
        =========
            value: otro valor de la operacion

        Returns
        ==========
            Infinite : si value de tipo Infinite y tienen el mismo signo.
            
            Indeterminate : si value es de tipo Infinite con distinto signo.

            NotImplemented : Si algo sale mal.

        """

        if isinstance(value, Infinite):
            if self.sign == value.sign:
                return self
            
            from indeterminate import Indeterminate

            return Indeterminate()
        
        from null import Null

        if isinstance(value, Null):
            return self
        
        try:
            return self 
        except TypeError:
            return NotImplemented
    
    def __radd__(self, value):

        """Operacion value + Infinite.

        Parameters
        =========
            value: otro valor de la operacion

        Returns
        ==========
            Infinite : si value de tipo Infinite y tienen el mismo signo.
            
            Indeterminate : si value es de tipo Infinite con distinto signo.

            NotImplemented : Si algo sale mal.

        """

        return self.__add__(value)
    
    def __sub__(self, value):

        """Operacion Infinite - value.

        Parameters
        =========
            value: otro valor de la operacion

        Returns
        ==========
            Infinite : si value es de tipo Null, int o float.
            
            Indeterminate : si value es de tipo Infinite con mismo signo.

            NotImplemented : si value no es de un tipo compatible.

        """
        
        if isinstance(value, Infinite):
            return self.__add__(Infinite(-value.sign))
        
        from null import Null

        if isinstance(value, (Null, int, float)):
            return self
        return NotImplemented
    
    def __rsub__(self, value):

        """
        Operacion value - Infinite

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =========

        Infinite : si value es de tipo Null, int o float.

        NotImplemented : en otro caso 
        """

        from null import Null

        if isinstance(value, (Null, int, float)):
            return Infinite(-self.sign)
        
        return NotImplemented
    
    def __mul__(self, value):
        
        """
        Operacion Infinite * value

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =======

        Indeterminate : Si value es Null o 0.

        Infinite : Si value es infinite, int o float se multiplican los signos. 

        NotImplemented : Si value es de un tipo incompatible
        """

        from null import Null

        if isinstance(value, Null) or value == 0:
            from indeterminate import Indeterminate

            return Indeterminate()
        
        if isinstance(value, Infinite):
            new_sign = self.sign * value.sign
            return Infinite(new_sign)
        
        if isinstance(value, (int,float)):
            new_sign = self.sign * (1 if value > 0 else -1)
            return Infinite(new_sign)
        
        return NotImplemented
    
    def __rmul__(self, value):

        """
        Operacion value * Infinite

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =======

        Indeterminate : Si value es Null o 0.

        Infinite : Si value es infinite, int o float se multiplican los signos. 

        NotImplemented : Si value es de un tipo incompatible
        """

        return self.__mul__(value)
    
    def __truediv__(self, value):

        """
        Operacion Infinite / value

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =======

        Indeterminate : Si value es de tipo Infinite.

        Infinite : si value es Null o 0.

        Infinite : si value es int o float se multiplican los signos.

        NotImplemented : Si value es de un tipo incompatible
        """

        if isinstance(value, Infinite):
            from indeterminate import Indeterminate

            return Indeterminate()
        
        from null import Null

        if isinstance(value, Null) or value == 0:
            return self.__pos__()
        
        if isinstance(value, (int, float)):
            new_sign = self.sign * (1 if value > 0 else -1)
            return Infinite(new_sign)
        
        return NotImplemented
    
    def __rtruediv__(self, value):

        """
        Operacion value / Infinite

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =======

        Indeterminate : Si value es Indeterminate (propagacion).

        Null: si value es de tipo Null, int o float.

        NotImplemented : Si value es de un tipo incompatible.
        """

        from indeterminate import Indeterminate    

        if isinstance(value, Indeterminate):
            return value
        
        from null import Null
        
        if isinstance(value, (Null, int, float)):
            return Null()
            
        return NotImplemented
    
    def __mod__(self, value):

        """
        Operacion Infinite % value

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =======

        Indeterminate : Si value es Indeterminate (propagacion).

        Indeterminate: si value es de tipo Null, Infinite o int o float.

        NotImplemented : Si value es de un tipo incompatible.
        """

        from indeterminate import Indeterminate

        if isinstance(value, Indeterminate):
            return value
        
        from null import Null

        if not isinstance(value, (Null, int, float, Infinite)):
            return NotImplemented

        return Indeterminate()
    
    def __rmod__(self, value):

        """
        Operacion value % Infinite

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =======

        Indeterminate : Si value es Indeterminate (propagacion).

        Null : si value es de tipo Null.

        int : si value es de tipo int.

        float : si value es de tipo float.

        NotImplemented : Si value es de un tipo incompatible.
        """

        from null import Null
        from indeterminate import Indeterminate

        if isinstance(value, (Indeterminate, Null, int, float)):
            return value
        
        return NotImplemented
    
    def __pow__(self, value):

        """
        Operacion Infinite ^ value

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =========

        Indeterminate : Si value es de tipo  Infinite, Null o 0.

        Null : Si value es < 0.

        Infinite : si value es impar y el signo de infinite es negativo.

        Infinite(-1) : si value es par o el signo de infinite es positivo.

        NotImplemented : Si value es de un tipo incompatible.
        
        """

        if isinstance(value, Infinite):
            from indeterminate import Indeterminate

            return Indeterminate()
        
        from null import Null
        
        if isinstance(value, Null) or value == 0:
            return Indeterminate()
        
        if isinstance(value, (float, int)):
            if value > 0:
                new_sign = 1
                if self.sign == -1 and value % 2 != 0:
                    new_sign = -1

                return Infinite(new_sign)

            if value < 0:
                return Null()

        return NotImplemented
    
    def __rpow__(self, value):

        """
        Operacion value ^ Infinite

        Parameters
        ==========

        value : otro valor de la operacion

        Returns
        =========

        Indeterminate : Si value es de tipo Indeterminate o 1.

        Null : Si value es Null y el signo de infinite es positivo.

        Infinite : Si value es de tipo Null y el signo de infinite de negativo.

        Infinite(1) : Si el signo de infinite es positivo y value > 1.

        Null : Si el signo de infinite es positivo y 0 <= value < 1.

        Null : si el signo de infinite es negativo y value > 1.

        Infinite(1) : Si el signo de infinite es negativo y 0 <= value < 1.

        NotImplemented : Si value es de un tipo incompatible.
        
        """

        from indeterminate import Indeterminate
        from null import Null

        if isinstance(value, Indeterminate):
            return value
        
        if isinstance(value, Null):
            if self.sign == 1:
                return value
            else:
                return Infinite(1)
            
        if value == 1:
            return Indeterminate()
        
        if self.sign == 1:
            if value > 1:
                return self
            elif 0 <= value < 1:
                return Null()
            return NotImplemented
        
        if self.sign == -1:
            if value > 1:
                return Null()
            elif 0 <= value < 1:
                return Infinite(1)
            return NotImplemented
        
        return NotImplemented
    
    def print(self, _):

        """
        Imprime el valor del objeto Infinite en el scope dado

        Args:
            scope: scope donde se imprime el valor
        """

        return self.__str__()
    
    def to_bool(self):

        from .bool import Bool

        """
        Convierte el objeto Infinite a Bool
        Returns:
            Bool: True
        """

        return Bool(True)
    
    def to_number(self):

        raise TypeError("El objeto infinite no se puede expressar como un numero")
    
    def to_text(self):

        from .text import Text

        """
        Convierte el objeto Infinite a Text
        Returns:
            Text: representacion en texto del infinito
        """

        return Text(self.__str__())
    
    def to_expresion(self):

        from .expresion import Expresion

        """
        Convierte el objeto Infinite a Expression
        Returns:
            Expresion: representacion en expresion del infinito
        """

        return Expresion(self.__str__())
    
    def to_set(self):

        from .set import HezaSet

        """
        Convierte el objeto Infinite a Set
        Returns:
            HezaSet: conjunto con el infinito como unico elemento
        """

        return HezaSet([self])
    
    def to_infinite(self):

        """
        Convierte el objeto Infinite a Infinite
        Returns:
            Infinite: mismo objeto
        """

        return self
    
    def to_null(self):

        from .null import Null

        """
        Convierte el objeto Infinite a Null
        Returns:
            Null: objeto Null
        """

        return Null()
    
    def to_indeterminate(self):

        from .indeterminate import Indeterminate

        """
        Convierte el objeto Infinite a Indeterminate
        Returns:
            Indeterminate: objeto Indeterminate
        """

        return Indeterminate()
    
    def to_abs(self):

        """
        Convierte el objeto Infinite a su valor absoluto
        Returns:
            Infinite: infinito positivo
        """

        return Infinite(1)