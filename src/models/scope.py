from .block import Block

class Scope:

    """
    Clase Scope de Heza
    
    Attributes:
        vars (dict): Diccionario de variables
        functions (list): Lista de Funciones
        traces (list): Lista de Traces
        parent (Scope|None): Scope padre
    """

    def __init__(self, parent = None):
        self.vars = {}
        self.functions = {}
        self.blocks = {}
        self.traces = {}
        self.objects = {}
        self.parent = parent
        self.mode = None

    def __repr__(self):
        return str(self.vars)
    
    def add_object(self, name, obj):
        self.objects[name] = obj

    def get_object(self, name):
        if name in self.objects:
            return self.objects[name]
        if self.parent:
            return self.parent.get_object(name)
        raise ValueError(f"Objeto '{name}' no definido")

    def add_block(self, name, args, body):
        from .block import Block
        self.blocks[name] = Block(args, body)

    def get_block(self, name) -> Block:

        if name in self.blocks:
            return self.blocks[name]

        if not self.parent:
            raise NameError(f"El bloque {name} no existe")

        return self.parent.get_block(name)

    def is_block_here(self, name):
        if name in self.blocks:
            return True

        if not self.parent:
            False

        return self.parent.get_block(name)

    def add_function(self, name, args, expresion):
        from .function import Function
        self.functions[name] = Function(args, expresion)

    def is_function_here(self, name): 

        if name in self.functions:
            return True
        
        if not self.parent:
            return False
        
        return self.parent.is_function_here(name)
    
    def get_function(self, name):
        if name in self.functions:
            return self.functions[name]
        
        if not self.parent:
            raise NameError(f"La funcion {name} no existe")
        
        return self.parent.get_function(name)

    def add_var(self, name, value, initial = True, in_this = False) -> bool:

        if name in self.vars or in_this:

            self.vars[name] = value
            return True
        
        else:

            exists = False

            if self.parent:
                exists = self.parent.add_var(name, value, False)

            if not initial:
                return exists
            
            elif not exists:
                self.vars[name] = value
                return True

    def modify_var(self, var_name, new_value):

        if var_name in self.traces:
            self.traces[var_name].add(new_value)

        if var_name in self.vars:

            self.vars[var_name] = new_value
            return

        if not self.parent:
            raise NameError(f"La variable '{var_name}' no esta declarada")

        self.parent.modify_var(var_name, new_value)

    def get_trace(self, var_name):

        if var_name  in self.traces:

            from models import HezaSet

            return HezaSet(self.traces[var_name].values.copy())

        if not self.parent:
            raise NameError(f"La variable {var_name} no contiene trace")
        
        return self.parent.get_trace(var_name)

    def get_value(self, var_name, interpreter, son = None):

        """
        Metodo para obtener el valor de un identificador

        Parameters
        =======
            var_name (str) : Nombre de la variable
            son (Scope|None) : Scope hijo para obtener el valor de forma recuersiva en caso de Reference

        Returns
        =======
            Any : El valor obtenido en el scope actual o en el padre

        Raises
        =======
            NameError : La variable no existe
        """

        from models import Reference

        if var_name in self.vars:
            value = self.vars[var_name]
            if isinstance(value, Reference):
                return value.evaluate(son if son else self, interpreter)
            return value
        
        try:
            obj_ref = self.get_object(var_name)
            from .object import HezaObject
            return HezaObject(obj_ref.attributes.copy(), obj_ref.name)
        except ValueError:
            pass

        # Luego bloques
        if var_name in self.blocks:
            from .block import HezaBlock
            return HezaBlock(self.blocks[var_name], self)

        # Luego funciones matemáticas
        if var_name in self.functions:
            from .function import HezaFunction
            return HezaFunction(self.functions[var_name], self)

        if self.parent:
            return self.parent.get_value(var_name, interpreter, self)
        raise ValueError(f"Variable {var_name} no declarada")

    def type_from_name(self, var_name):

        if var_name not in self.vars:
            raise ValueError(f"Variable {var_name} no declarada")

        return type(self.vars[var_name])
    
    def get_all_symbols(self):

        return {
            "vars": self.vars,
            "funs": self.functions,
            "blocks": self.blocks,
            "objects": self.objects
        }