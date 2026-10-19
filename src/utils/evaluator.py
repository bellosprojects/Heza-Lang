from models import *
import engine.HSCE as cas

def evaluate_node_from_scope(node: dict, scope : Scope, interpreter = None) -> cas.Expr:

    type = node['type']

    # if type == 'expresion':
    #     return Expresion(node['value'])

    if type == 'id':
        return scope.get_value(node['value'], interpreter)

    elif type == 'number':
        return cas.Number(node['value'])
    
    elif type == 'text':
        return cas.TextExpr(node['value'])

    elif type == 'bool':
        return cas.BoolExpr(node['value'])

    elif type == 'null':
        return cas.NullExpr()

    elif type == 'infinite':
        return Infinite()

    elif type == 'indeterminate':
        return Indeterminate()
    
    elif type == 'reference':
        return Reference(node['expresion'])
    
    elif type == 'tuple':
        values = [evaluate_node_from_scope(v, scope, interpreter) for v in node['values']]
        
        return cas.TupleExpr(values)

    elif type == 'set':
        values = [evaluate_node_from_scope(value, scope, interpreter) for value in node['value']]

        return cas.FiniteSet(values)
    
    elif type == 'range':
        from_value = evaluate_node_from_scope(node['fromValue'], scope, interpreter)
        to_value = evaluate_node_from_scope(node['toValue'], scope, interpreter)
        step = node.get('step', None)
        if step:
            step = evaluate_node_from_scope(node['step'], scope, interpreter)
        return heza_range(from_value, to_value, step)

    elif type == 'eval':
        return eval(
            evaluate_node_from_scope(node['expresion'], scope, interpreter),
            node['var'],
            evaluate_node_from_scope(node['value'], scope, interpreter)
            )

    elif type == 'totalEval':
        return evaluate_node_from_scope(node['expresion'], scope, interpreter).to_expresion()

    elif type == 'derivative':
        return derivative(
            evaluate_node_from_scope(node['expresion'], scope, interpreter),
            node['var']
        )
    
    elif type == 'indefiniteIntegral':
        
        expresion = evaluate_node_from_scope(node['expresion'], scope, interpreter).to_expresion()

        return expresion.integrate(node['var'])
    
    elif type == 'definiteIntegral':
        expresion = evaluate_node_from_scope(node['expresion'], scope, interpreter).to_expresion()
        limI = evaluate_node_from_scope(node['limI'], scope, interpreter)
        limS = evaluate_node_from_scope(node['limS'], scope, interpreter)

        return expresion.integrate_definite(node['var'], limI, limS)

    elif type == 'convertion':
        return convertions(
            evaluate_node_from_scope(node['value'], scope, interpreter),
            node['toType']
        )
    
    elif type == 'attribute_access':
        obj = evaluate_node_from_scope(node['object'], scope, interpreter)
        from models.object import HezaObject
        if not isinstance(obj, HezaObject):
            raise TypeError("Se intenta acceder a un atributo de un objeto que no es HezaObject")
        return obj.get(node['attribute'])
    
    elif type == 'object_instantiation':
        from models.object import HezaObject
        obj_name = node['object_name']
        try:
            obj_def = scope.get_object(obj_name)
        except ValueError:
            raise ValueError(f"El objeto '{obj_name}' no está definido")
        # Crear una copia independiente
        new_obj = HezaObject(obj_def.attributes.copy(), obj_name)
        # Asignar los valores dados
        for attr_name, attr_ast in node['attr_values'].items():
            if attr_name not in new_obj.attributes:
                raise AttributeError(f"El objeto '{obj_name}' no tiene el atributo '{attr_name}'")
            val = evaluate_node_from_scope(attr_ast, scope, interpreter)
            new_obj.set(attr_name, val)
        return new_obj
    
    elif type == 'method_call':
        from models.object import HezaObject
        obj = evaluate_node_from_scope(node['object'], scope, interpreter)
        if not isinstance(obj, HezaObject):
            raise TypeError("Se intenta llamar un método de un objeto que no es HezaObject")
        method = obj.get(node['method_name'])
        if not hasattr(method, 'call'):
            raise TypeError(f"El atributo '{node['method_name']}' no es una función invocable")
        args = [evaluate_node_from_scope(arg, scope, interpreter) for arg in node['args']]
        return method.call(args, interpreter)

    elif type == 'constantText':
        return constantText(node['value'])

    elif type == 'binaryOperation':
        left = evaluate_node_from_scope(node['leftValue'], scope, interpreter)
        right = evaluate_node_from_scope(node['rightValue'], scope, interpreter)
        return binary_operation(left, right, node['operation'])

    elif type == 'unaryOperation':
        value = evaluate_node_from_scope(node['value'], scope, interpreter)
        return unary_operation(value, node['operation'])
    
    elif type == 'call':
        # Obtener el callee (puede ser un ID, attribute_access, etc.)
        callee_node = node['callee']
        args = [evaluate_node_from_scope(arg, scope, interpreter) for arg in node['args']]
        # Evaluar el callee para obtener el objeto invocable
        
        try:
            callee = evaluate_node_from_scope(callee_node, scope, interpreter)
            # Evaluar argumentos
            # Si el callee es invocable, llamarlo
            if hasattr(callee, 'call'):
                return callee.call(args, interpreter)

            else:
                raise TypeError(f"El objeto {callee} no es invocable")

        except ValueError:
            
            if callee_node['type'] == 'id':
                func_name = callee_node['value']
                
                import utils.funs as funs

                if hasattr(funs, func_name):
                    func = getattr(funs, func_name)
                    return func(*args)
                else:
                    raise NameError(f"Funcion '{func_name}' no definida")

            else:
                raise

    elif type == 'ternaryOperation':
        condicion = evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool()
        return evaluate_node_from_scope(node['trueValue'], scope, interpreter) if condicion else evaluate_node_from_scope(node['falseValue'], scope, interpreter) 
    
    elif type == 'abs':

        obj = evaluate_node_from_scope(node['value'], scope, interpreter)

        if isinstance(obj, cas.TextExpr):
            return obj.length()

        else:
            raise TypeError("La operacion || no soporta este tipo de dato")

    elif type == 'access':
        index = evaluate_node_from_scope(node['index'], scope, interpreter)
        if not isinstance(index, cas.Number):
            raise TypeError("El parametro index debe ser un numero")

        obj = evaluate_node_from_scope(node['set'], scope, interpreter)

        if isinstance(obj, cas.TextExpr):
            return obj.charAt(index)

        elif isinstance(obj, HezaTuple):
            if not isinstance(index, Number):
                raise ValueError("Índice debe ser un número")
            idx = int(index.value)
            if idx < 0 or idx >= len(obj.values):
                raise IndexError("Índice fuera de rango")
            return obj.get(index)
        elif isinstance(obj, HezaTuple):
            index = evaluate_node_from_scope(node['index'], scope, interpreter)
            return obj.get(index)
        else:
            raise TypeError("Se esperaba un Conjunto o una Tupla para acceder")

    elif type == 'foreach':
        set_ = evaluate_node_from_scope(node['set'], scope, interpreter)
        iter_vars = node['descompose']
        pattern = iter_vars['type']
        
        if not isinstance(set_, HezaSet):
            raise ValueError("Se esperaba un Conjunto para recorrer en foreach")
        
        final = True
        for i in set_.values:

            scope = Scope(scope)

            if pattern == 'normal':
                scope.add_var(iter_vars['vars'][0], i)
            elif pattern == 'tuple':
                if not isinstance(i, HezaTuple):
                    raise TypeError("Solo se puede decomponer una tupla")

                if len(i.values) != len(iter_vars['vars']):
                    raise TypeError("Para descomponer el elemento debe tener la misma cantidad de variables que lo esperan")

                for idx, var in enumerate(iter_vars['vars']):
                    scope.add_var(var, i.values[idx])

            final = final and evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool()
            scope = scope.parent
            if not final:
                break

        return Bool(final)

    elif type == 'exist':
        set_ = evaluate_node_from_scope(node['set'], scope, interpreter)
        iter_vars = node['descompose']
        pattern = iter_vars['type']
        
        if not isinstance(set_, HezaSet):
            raise ValueError("Se esperaba un Conjunto para recorrer en foreach")
        
        final = False
        for i in set_.values:

            scope = Scope(scope)

            if pattern == 'normal':
                scope.add_var(iter_vars['vars'][0], i)
            elif pattern == 'tuple':
                if not isinstance(i, HezaTuple):
                    raise TypeError("Solo se puede decomponer una tupla")

                if len(i.values) != len(iter_vars['vars']):
                    raise TypeError("Para descomponer el elemento debe tener la misma cantidad de variables que lo esperan")

                for idx, var in enumerate(iter_vars['vars']):
                    scope.add_var(var, i.values[idx])

            final = final or evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool()
            scope = scope.parent
            if final:
                break

        return Bool(final)

    elif type == 'summation':
        set_ = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()
        iter_vars = node['descompose']
        pattern = iter_vars['type']

        final = cas.Number(0)
        for i in set_.values:

            scope = Scope(scope)

            if pattern == 'normal':
                scope.add_var(iter_vars['vars'][0], i)
            elif pattern == 'tuple':
                if not isinstance(i, (HezaTuple)):
                    raise ValueError("Solo se puede descomponer una tupla")

                if len(i.values) != len(iter_vars['vars']):
                    raise ValueError("Para descomponer el elemento debe tener la misma cantidad de variables que lo esperan")

                for idx, var in enumerate(iter_vars['vars']):
                    scope.add_var(var, i.values[idx])
                

            final += evaluate_node_from_scope(node['expresion'], scope, interpreter)

            scope = scope.parent

        return final

    elif type == 'production':
        set_ = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()
        iter_vars = node['descompose']
        pattern = iter_vars['type']

        final = Number(1)
        for i in set_.values:

            scope : Scope | None = Scope(scope)
            if pattern == 'normal':
                scope.add_var(iter_vars['vars'][0], i)
            elif pattern == 'tuple':
                if not isinstance(i, (HezaTuple)):
                    raise ValueError("Solo se puede descomponer una tupla")

                if len(i.values) != len(iter_vars['vars']):
                    raise ValueError("Para descomponer el elemento debe tener la misma cantidad de variables que lo esperan")

                for idx, var in enumerate(iter_vars['vars']):
                    scope.add_var(var, i.values[idx])

            final *= evaluate_node_from_scope(node['expresion'], scope, interpreter)

            scope = scope.parent

        return final

    elif type == 'filter':
        set_ = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()
        iter_vars = node['descompose']
        pattern = iter_vars['type']

        filtered_set = HezaSet(EMPYSET)

        scope = Scope(scope)

        for i in set_.values:

            scope = Scope(scope)

            if pattern == 'normal':
                scope.add_var(iter_vars['vars'][0], i)
            elif pattern == 'tuple':
                if not isinstance(i, HezaTuple):
                    raise ValueError("Se se puede descomponer una tupla")
                if len(i.values) != len(iter_vars['vars']):
                    ValueError("Para descomponer el elemento debe tener la misma cantidad de variables que lo esperan")

                for idx, var in enumerate(iter_vars['vars']):
                    scope.add_var(var, i.values[idx])

            if evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool():
                filtered_set.add(i)

            scope = scope.parent

        return filtered_set
    
    elif type == 'transformation':
    
        set_ = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()
        iter_vars = node['descompose']
        pattern = iter_vars['type']

        final_set = HezaSet(EMPYSET)

        for i in set_.values:

            scope = Scope(scope)

            if pattern == 'normal':
                scope.add_var(iter_vars['vars'][0], i)
            elif pattern == 'tuple':
                if not isinstance(i, HezaTuple):
                    raise ValueError("Se se puede descomponer una tupla")
                if len(i.values) != len(iter_vars['vars']):
                    ValueError("Para descomponer el elemento debe tener la misma cantidad de variables que lo esperan")

                for idx, var in enumerate(iter_vars['vars']):
                    scope.add_var(var, i.values[idx])

            if evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool():
                final_set.add(evaluate_node_from_scope(node['expresion'], scope, interpreter))

            scope = scope.parent

        return final_set
    
    elif type == 'lim':
        
        expresion = evaluate_node_from_scope(node['expresion'], scope, interpreter).to_expresion()

        lim = expresion.limit(node['var'], node['value'], node['dir'])

        return lim

    elif type == 'abstract':

        definition = node.get('definition')

        if definition == 'set':
            return cas.AbstractSet('A')

        if definition == 'scalar':
            return cas.Variable('x')

    else:
        return cas.NullExpr()

def heza_range(limI, limS, step):
    if not isinstance(limI, Number):
        raise ValueError("from_value en Range debe ser numerico")

    if not isinstance(limS, Number):
        raise ValueError("to_value en Range debe ser numerico")

    if not step:
        step = Number(1) if limI.value < limS.value else Number(-1)

    if not isinstance(step, Number):
        raise ValueError("step en Range debe ser numerico")

    if step.value == 0:
        raise ValueError("step en range no puede ser 0")

    if step.value > 0 and limI.value > limS.value:
        raise ValueError("step en range no puede ser mayor a 0 si from_value > to_value")

    if step.value < 0 and limI.value < limS.value:
        raise ValueError("step en range no puede ser menor a 0 si from_value < to_value")

    final_range = HezaSet(EMPYSET)

    initial_value = limI.value

    if limI.value < limS.value:
        while initial_value <= limS.value:
            final_range.add(Number(initial_value))
            initial_value += step.value

    else:
        while initial_value >= limS.value:
            final_range.add(Number(initial_value))
            initial_value += step.value

    return final_range

def derivative(expresion, var):
    if isinstance(expresion, Expresion):
        return expresion.derive(var)
    raise ValueError("La funcion eval solo aplica para objetos de tipo Expresion")

def eval(expresion, var, value):
    if isinstance(expresion, Expresion):
        return expresion.evaluate(var, value)
    raise ValueError("La funcion eval solo aplica para objetos de tipo Expresion")

def convertions(value: cas.Expr, type: dict) -> cas.Expr:

    convertions_allowed = [
        'Number',
        'Text',
        'Expresion',
        'Null',
        'Inf',
        'Nah',
        'Set',
        'Bool'
    ]

    if type.get('type') != 'id':
        raise ValueError("Se esperaba un identificador como tipo futuro")
    
    tipo = type.get('value')

    if tipo not in convertions_allowed:
        raise ValueError(f"El tipo '{tipo}' no es un tipo permitido para conversion")
    
    if tipo == 'Number':
        return value.to_number()

    elif tipo == 'Text':
        return cas.TextExpr(value.__str__())

    elif tipo == 'Expresion':
        return value.to_expresion()
    
    elif tipo == 'Set':
        if isinstance(value, cas.SetExpr):
            return value
        else:
            return cas.FiniteSet([value])
    
    elif tipo == 'Inf':
        return value.to_infinite()
    
    elif tipo == 'Nah':
        return value.to_indeterminate()
    
    elif tipo == 'Null':
        return cas.NullExpr()
        
    elif tipo == 'Bool':
        return value.to_bool()

    raise RuntimeError("Conversion no soportada")

def constantText(value: str) -> cas.TextExpr:
    if value == 'ESP':
        return cas.TextExpr(" ")

    elif value == 'LINE':
        return cas.TextExpr("\n")

    return cas.TextExpr("\t")

def binary_operation(left: cas.Expr, right: cas.Expr, operation: str) -> cas.Expr:

    # ============================================= TEXTOS =================================================================== #

    # CONCATENACION DE STRING
    if operation == 'add' and isinstance(left, cas.TextExpr) and isinstance(right, cas.TextExpr):
        return left + right

    # REPETICION DE STRINGS
    elif operation == 'mul':
        if isinstance(left, cas.TextExpr) and isinstance(right, cas.Number):
            return left * right
        elif isinstance(left, cas.Number) and isinstance(right, cas.TextExpr):
            return right * left

    # ====================================================== Aritmeticos ================================================================== #

    if operation in ['add', 'sub', 'div', 'mod', 'mul', 'pow']:

        if not isinstance(left, cas.Arithmetics):
            raise TypeError(f"El operando izquierdo debe ser del tipo Arithmetic para la operacion {operation}")
        if not isinstance(right, cas.Arithmetics):
            raise TypeError(f"El operando derecho debe ser del tipo Arithmetic para la operacion {operation}")

        if operation == 'add':
            return left + right
        elif operation == 'sub':
            return left + (cas.Number(-1) * right)
        elif operation == 'mul':
            return left * right
        elif operation == 'div':
            return left * (right ** cas.Number(-1))
        elif operation == 'mod':
            raise NotImplemented


    # =============================================== Boolenos ============================================================== # 

    if operation in ['equal', 'nEqual', 'greater', 'greaterEqual', 'less', 'lessEqual', 'in', 'notin', 'then', 'and', 'or']:


        if operation == 'equal':
            return  Bool(left == right)
        
        if operation == 'nEqual':
            return Bool(left != right)
        
        if operation == 'greater':
            return Bool(left > right)
        
        if operation == 'greaterEqual':
            return Bool(left >= right)
        
        if operation == 'less':
            return Bool(left < right)
        
        if operation == 'lessEqual':
            return Bool(left <= right)
        
        if operation == 'in':
            if not isinstance(right, cas.SetExpr):
                raise ValueError("La expresion right del operador in debe ser un conjunto")
            return cas.BoolExpr(right.contains(left))

        if operation == 'notin':
            if not isinstance(right, cas.SetExpr):
                raise ValueError("La expresion right del operador in debe ser un conjunto")
            return cas.BoolExpr(right.contains(left)).nott()
        
        if operation == 'del':
            if not isinstance(left, HezaSet):
                raise ValueError("La expresion left del operador del debe ser un conjunto")
            return left.eliminar(right)

        if not isinstance(left, cas.LogicExpr):
            raise TypeError("El operando izquierdo debe ser Logico")
        if not isinstance(right, cas.LogicExpr):
            raise TypeError("El operando derecho debe ser Logico")
        
        if operation == 'then':
            return left.nott().orr(right)

        if operation == 'and':
            return left.andd(right)

        if operation == 'or':
            return left.orr(right)


    # ========================================== CONJUNTOS ========================================================= #
    
    if operation in ['union', 'inter', 'symDiff', 'cross', 'sub']:

        CONVERSION_MAP = {
            'union': 'Union',
            'inter': 'Intersection',
            'symDiff': 'Symetric Difference',
            'cross': 'Cartesian Product',
            'sub': 'Substraction'
        }

        if not isinstance(left, cas.SetExpr):
            raise TypeError(f"El operando izquierdo debe ser un conjunto para la operacion {CONVERSION_MAP[operation]}")
        if not isinstance(right, cas.SetExpr):
            raise TypeError(f"El operando derecho debe ser un conjunto para la operacion {CONVERSION_MAP[operation]}")

        if operation == 'union':
            return left.union(right)
        elif operation == 'inter':
            return left.intersection(right)
        elif operation == 'symDiff':
            return left.symetric_difference(right)
        elif operation == 'cross':
            return left.cartesian_product(right)
        elif operation == 'sub':
            return left.difference(right)

    # =========================================== TUPLAS (VECTORES) =========================================================== #

    if operation in ['dot', 'mul' 'cross']:

        if isinstance(left, cas.TupleExpr):
            pass

        raise NotImplemented

    raise RuntimeError("Operacion binaria desconocida")

def unary_operation(value: cas.Expr, operation) -> cas.Expr:

    if operation == 'neg':

        if isinstance(value, cas.Arithmetics):
            return -value
        else:
            raise TypeError("La operacion -() solo se puede aplicar a operaciones de tipo Arithmetics")

    if operation == 'pos':
        return value

    if operation == 'fact':
        if not isinstance(value, Number):
            raise ValueError("La operacion fact(!) solo soporta valores de tipo Number")

        from math import factorial
        return cas.Number(factorial(int(value.value)))

    if operation == 'sqrt':
        if not isinstance(value, cas.Number):
            raise ValueError("La operacion sqrt(√) solo soporta valores de tipo Number")

        from math import sqrt
        return cas.Number(sqrt(value.to_double()))

    if operation == 'not':

        if isinstance(value, cas.LogicExpr):
            return value.nott()

        else:
            raise TypeError("La operacion not solo se puede aplicar a expresiones logicas")

    raise RuntimeError("Operacion unaria desconocida")