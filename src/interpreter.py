import models as models
from utils import *
import math
from utils.evaluator import evaluate_node_from_scope
import sys

class InterpreterError(Exception):
    pass

class Interpreter:

    def __init__(self, ast):
        self.ast = ast
        self.scope = models.Scope(None)
        self.sep = ""
        self.extra = 0
        self.sys = models.Sys()
        self.create_init_vars()
        sys.setrecursionlimit(100000)

    def run(self):

        for command in self.ast['program']:
            self.process(command)

    def create_object_from_external_module(self, external_ast, alias, name):

        external_scope = models.Scope()

        for command in external_ast['program']:
            
            tipo = command['type']

            if tipo == 'function':
                
                self.function(command=command, scope=external_scope)

            elif tipo == 'fun':
                self.fun(command=command, scope=external_scope)

            elif tipo == 'object_declaration':
                self.object_declaration(command=command, scope=external_scope)

            elif tipo == 'asignacion':
                self.asignacion(command=command, scope=external_scope)

        attrs = {}
        symbols = external_scope.get_all_symbols()

        for fun_name, fun in symbols['funs'].items():
            attrs[fun_name] = models.HezaFunction(fun, external_scope)

        for block_name, block in symbols['blocks'].items():
            attrs[block_name] = models.HezaBlock(block, external_scope)

        for obj_name, obj in symbols['objects'].items():
            attrs[obj_name] = obj

        for var_name, var in symbols['vars'].items():
            attrs[var_name] = var

        external_object = models.HezaObject(attributes=attrs, name=name)
        self.scope.add_object(name=name, obj=external_object)

        instance_var_value_node = {
            "type": "id",
            "value": name
        }

        procesed_value = evaluate_node_from_scope(instance_var_value_node, scope=self.scope, interpreter=self)

        self.scope.add_var(name=alias, value=procesed_value)

    def create_init_vars(self):
        self.scope.add_var("e", models.Number(math.e))
        self.scope.add_var("pi", models.Number(math.pi))
        self.scope.add_var("LINE", models.Text("\n"))

    def process(self, command):
        type = command['type']

        if type == 'asignacion':
            self.asignacion(command)
        elif type == "sys.out":
            self.print(command)
        elif type == 'reassignment':
            self.reassignment(command)
        elif type == 'sys.in':
            self.input(command)
        elif type == 'sys.set':
            prop = evaluate_node_from_scope(command['property'], self.scope, self)
            value = evaluate_node_from_scope(command['value'], self.scope, self)
            if isinstance(prop, models.Text):
                self.sys.set(prop.value, value)
            else:
                raise ValueError("La propiedad de Sys.set debe ser un texto")
        elif type == 'conditional':
            value = self.conditional(command)
            if value:
                return value
        elif type == 'while':
            value = self.while_(command)
            if value:
                return value
        elif type == 'stop':
            return models.Control("stop")
        elif type == 'for':
            value = self.for_(command)
            if value:
                return value
        elif type == 'copy':
            self.copy(command)
        elif type == 'add':
            self.add(command)
        elif type == 'call':
            evaluate_node_from_scope(command, self.scope, self)
        elif type == 'function':
            self.function(command)
        elif type == 'fun':
            self.fun(command)
        elif type == 'use':
            self.use(command)
        elif type == 'return':
            return self.return_(command)
        elif type == 'sys.clear':
            sys.stdout.write("\033[2J\033[H")
        elif type == 'object_declaration':
            self.object_declaration(command)
        elif type == 'expression_statement':
            # Evaluamos y descartamos el resultado
            evaluate_node_from_scope(command['expression'], self.scope, self)
        elif type == 'selectiveUse':
            self.selectiveUse(command)

    def object_declaration(self, command, scope: models.Scope | None = None):

        insert_scope = scope if scope else self.scope

        attrs = {}
        for attr_name, default_ast in command['attributes'].items():
            default_val = evaluate_node_from_scope(default_ast, insert_scope, self)
            attrs[attr_name] = default_val
        obj = models.HezaObject(attrs, command['name'])
        insert_scope.add_object(command['name'], obj)
        
    def return_(self, command):
        return_value = None
        values = [evaluate_node_from_scope(val, self.scope, self) for val in command['values']]
        if len(values) > 1:
            from src.models.tuple import HezaTuple
            return_value = HezaTuple(values)
        elif len(values) == 1:
            return_value = values[0]
        else:
            return_value = models.Null()

        return models.Control("return", return_value)
        
    def insert_selective_symbols_from_external_module(self, module_ast: dict, symbols: list[str]):
        external_scope = models.Scope()

        for command in module_ast['program']:
            
            tipo = command['type']

            if tipo == 'function':
                
                self.function(command=command, scope=external_scope)

            elif tipo == 'fun':
                self.fun(command=command, scope=external_scope)

            elif tipo == 'object_declaration':
                self.object_declaration(command=command, scope=external_scope)

            elif tipo == 'asignacion':
                self.asignacion(command=command, scope=external_scope)

        all_symbols = external_scope.get_all_symbols()

        for fun_name, fun in all_symbols['funs'].items():
            if fun_name in symbols:
                self.scope.add_var(fun_name, models.HezaFunction(fun, external_scope), in_this=True)

        for block_name, block in all_symbols['blocks'].items():
            if block_name in symbols:
                self.scope.add_var(block_name, models.HezaBlock(block, external_scope), in_this=True)

        for obj_name, obj in all_symbols['objects'].items():
            if obj_name in symbols:
                self.scope.add_object(obj_name, obj)

        for var_name, var in all_symbols['vars'].items():
            if var_name in symbols:
                self.scope.add_var(var_name, var)

    def selectiveUse(self, command):
        from lexer import Lexer
        from src.parser import Parser
        import os

        module_name = command['module'] + '.hz'

        if not os.path.exists(module_name):
            raise

        module_code = open(module_name, encoding='UTF-8').read()
        module_tokens = Lexer(module_code).tokenize()
        module_ast = Parser(module_tokens).parse_program()
        self.insert_selective_symbols_from_external_module(module_ast=module_ast, symbols=command.get('symbols', []))

    def use(self, command):

        from lexer import Lexer
        from src.parser import Parser
        import os

        module_name = command['module'] + '.hz'

        if not os.path.exists(module_name):
            raise

        module_code = open(module_name, encoding='UTF-8').read()
        module_tokens = Lexer(module_code).tokenize()
        module_ast = Parser(module_tokens).parse_program()
        self.create_object_from_external_module(module_ast, command['alias'], command['module'])

    def print(self, command):

        values = [evaluate_node_from_scope(value, self.scope, self) for value in command['values']]

        self.sys.print(values)

    def fun(self, command, scope: models.Scope | None = None):
        insert_scope = scope if scope  else self.scope
        insert_scope.add_block(command['name'], command['params'], command['body'])

    def function(self, command, scope: models.Scope | None = None):
        insert_scope = scope if scope  else self.scope
        insert_scope.add_function(command['name'], command['args'], command['expresions'])

    def add(self, command):

        set_name = command['set']

        set = evaluate_node_from_scope(set_name, self.scope, self)

        if not isinstance(set, models.HezaSet):
            raise TypeError(f"Se esperaba un conjunto antes de ← y se encontro {type(set)}")

        values = command['values']

        new_values = [evaluate_node_from_scope(value, self.scope, self) for value in values]

        for value in new_values:
            set.add(value)

    def copy(self, commad):
        
        vars = commad['vars']

        for var in vars:

            value = self.scope.get_value(var_name=var, interpreter=self)

            self.scope.add_var(var, value, in_this=True)

    def asignacion(self, command, scope: models.Scope | None = None):

        insert_scope = scope if scope else self.scope
        
        lvalues = command['lvalues']
        value = evaluate_node_from_scope(command['value'], insert_scope, self)
        
        if len(lvalues) == 1:
            self._assign_lvalue(lvalues[0], value, insert_scope)
            return

        # Si el valor es una tupla o conjunto, desempaquetar
        if isinstance(value, (models.HezaTuple, models.HezaSet)):
            elements = value.values if isinstance(value, models.HezaTuple) else value.values
            if len(elements) != len(lvalues):
                raise ValueError(
                    f"El valor tiene {len(elements)} elementos, "
                    f"pero se esperaban {len(lvalues)}"
                )
            for lval, val in zip(lvalues, elements):
                self._assign_lvalue(lval, val, insert_scope)
        else:
            raise ValueError(
                f"Se esperaba una tupla o conjunto con {len(lvalues)} elementos, "
                f"pero se obtuvo {type(value).__name__}"
            )

    def _assign_lvalue(self, lvalue_node, value, scope : models.Scope):
        """Asigna un valor a un lvalue (id, attribute_access, access)"""
        if lvalue_node['type'] == 'id':
            var_name = lvalue_node['value']
            # Si la variable existe, la modifica; si no, la crea
            scope.add_var(var_name, value)
        elif lvalue_node['type'] == 'attribute_access':
            obj = evaluate_node_from_scope(lvalue_node['object'], scope, self)
            if not isinstance(obj, models.HezaObject):
                raise TypeError("El lado izquierdo no es un objeto")
            obj.set(lvalue_node['attribute'], value)
        elif lvalue_node['type'] == 'access':
            obj = evaluate_node_from_scope(lvalue_node['set'], scope, self)
            if not isinstance(obj, models.HezaSet):
                raise TypeError("Solo se puede asignar a un índice de un conjunto (no a una tupla)")
            index = evaluate_node_from_scope(lvalue_node['index'], scope, self)
            if not isinstance(index, models.Number):
                raise TypeError("El índice debe ser un número")
            idx = int(index.value)
            if idx < 0 or idx >= len(obj.values):
                raise IndexError("Índice fuera de rango")
            obj.values[idx] = value
        else:
            raise TypeError(f"Tipo de lvalue no soportado: {lvalue_node['type']}")

    def input(self, command):
        
        for var in command["vars"]:

            new_value = self.sys.get_from_console()

            if not self.scope.vars.get(var, None):
                self.scope.add_var(name=var, value=models.Null())

            self.scope.modify_var(var_name=var, new_value= models.Text(new_value))

    def conditional(self, command):

        branches = command.get('branches', [])
        else_body = command.get('else_body', None)

        for branch in branches:
            condition = evaluate_node_from_scope(branch['condition'], self.scope, self)
            if condition.to_bool():
                self.scope = models.Scope(self.scope)
                for inst in branch['body']:
                    value = self.process(inst)
                    if value is not None:
                        return value
                self.scope = self.scope.parent
                return  # Salir después de ejecutar el bloque

        # Si ninguna condición se cumple y hay else
        if else_body:
            self.scope = models.Scope(self.scope)
            for inst in else_body:
                value = self.process(inst)
                if value is not None:
                    return value
            self.scope = self.scope.parent

    def while_(self, command):

        body = command['body']

        while evaluate_node_from_scope(command['condition'], self.scope, self).to_bool():

            self.scope = models.Scope(self.scope)

            for inst in body:
                result = self.process(inst)
                if result is not None:
                    if isinstance(result, models.Control):
                        if result.type == 'return':
                            return result
                        elif result.type == 'stop':
                            self.scope = self.scope.parent
                            return None
                    else:
                        return result
                
            self.scope = self.scope.parent

    def for_(self, command):

        pattern_type = command.get('pattern_type', 'simple')
        vars_ = command['vars']
        sets = [evaluate_node_from_scope(set, self.scope, self) for set in command['sets']]
        body = command['body']

        if pattern_type == 'tuple':
            # Solo se espera un conjunto
            if len(sets) != 1:
                raise ValueError("El patrón de tupla requiere exactamente un conjunto")
            set_obj = sets[0]
            if not isinstance(set_obj, models.HezaSet):
                raise ValueError("Se esperaba un conjunto para iterar en ∀ con patrón de tupla")

            # Iterar sobre cada elemento del conjunto
            for element in set_obj.values:
                # Verificar que sea una tupla
                if not isinstance(element, models.HezaTuple):
                    raise ValueError(f"Cada elemento del conjunto debe ser una tupla, pero se obtuvo {type(element)}")
                # Verificar longitud
                if len(element.values) != len(vars_):
                    raise ValueError(
                        f"La tupla tiene {len(element.values)} elementos, "
                        f"pero se esperaban {len(vars_)}"
                    )
                # Crear ámbito y asignar variables
                self.scope = models.Scope(self.scope)
                for index, var in enumerate(vars_):
                    self.scope.add_var(var, element.values[index], in_this=True)
                # Ejecutar cuerpo
                for inter_command in body:
                    result = self.process(inter_command)
                    if result is not None:
                        if isinstance(result, models.Control):
                            if result.type == 'return':
                                return result
                            elif result.type == 'stop':
                                self.scope = self.scope.parent
                                return None
                        else:
                            return result
                self.scope = self.scope.parent
        else:
            # --- Comportamiento original: iteración por índices en paralelo ---
            for set_obj in sets:
                if not isinstance(set_obj, models.HezaSet):
                    raise ValueError("Todos los sets en for deben ser de tipo Conjunto")
            if len(sets) != len(vars_):
                raise ValueError("La cantidad de conjuntos y variables no coinciden")
            if len(sets) < 1:
                return
            length = sets[0].to_abs().value
            for set_obj in sets:
                if set_obj.to_abs().value != length:
                    raise ValueError("Todos los Conjuntos deben tener el mismo tamaño")
            for i in range(0, length):
                self.scope = models.Scope(self.scope)
                for index, var in enumerate(vars_):
                    self.scope.add_var(var, sets[index].get(models.Number(i)))
                for inter_command in body:
                    result = self.process(inter_command)
                    if result is not None:
                        if isinstance(result, models.Control):
                            if result.type == 'return':
                                return result
                            elif result.type == 'stop':
                                self.scope = self.scope.parent
                                return None
                        else:
                            return result
                self.scope = self.scope.parent


    def debug(self):
        print(self.scope)