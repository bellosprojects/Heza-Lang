from models import *

def evaluate_node_from_scope(node: dict, scope : Scope, interpreter = None):

    type = node['type']

    if type == 'expresion':
        return Expresion(node['value'])

    elif type == 'id':
        return scope.get_value(node['value'], interpreter)
    
    elif type == 'arroba':
        return Number(int(evaluate_node_from_scope(node['value'], scope, interpreter).value))

    elif type == 'number':
        return Number(node['value'])
    
    elif type == 'text':
        return Text(node['value'])

    elif type == 'bool':
        return Bool(node['value'])

    elif type == 'null':
        return Null()

    elif type == 'infinite':
        return Infinite()

    elif type == 'indeterminate':
        return Indeterminate()
    
    elif type == 'reference':
        return Reference(node['expresion'])
    
    elif type == 'trace':
        return scope.get_trace(node['value'])
    
    elif type == 'tuple':
        values = [evaluate_node_from_scope(v, scope, interpreter) for v in node['values']]
        
        return HezaTuple(values)

    elif type == 'set':
        values = [evaluate_node_from_scope(value, scope, interpreter) for value in node['value']]
        return HezaSet(values)
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
        from src.models.object import HezaObject
        if not isinstance(obj, HezaObject):
            raise TypeError("Se intenta acceder a un atributo de un objeto que no es HezaObject")
        return obj.get(node['attribute'])
    
    elif type == 'object_instantiation':
        from src.models.object import HezaObject
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
        from src.models.object import HezaObject
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
                
                import src.utils.funs as funs

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
        return evaluate_node_from_scope(node['value'], scope, interpreter).to_abs()

    elif type == 'access':
        obj = evaluate_node_from_scope(node['set'], scope, interpreter)
        if isinstance(obj, HezaSet):
            index = evaluate_node_from_scope(node['index'], scope, interpreter)
            if not isinstance(index, Number):
                raise ValueError("Índice debe ser un número")
            idx = int(index.value)
            if idx < 0 or idx >= len(obj.values):
                raise IndexError("Índice fuera de rango")
            return obj.get(index)   # usa obj, no set
        elif isinstance(obj, HezaTuple):
            index = evaluate_node_from_scope(node['index'], scope, interpreter)
            return obj.get(index)
        else:
            raise TypeError("Se esperaba un Conjunto o una Tupla para acceder")

    elif type == 'foreach':
        set = evaluate_node_from_scope(node['set'], scope, interpreter)
        
        if not isinstance(set, HezaSet):
            raise ValueError("Se esperaba un Conjunto para recorrer en foreach")
        
        final = True
        for i in set.values:

            scope = Scope(scope)
            scope.add_var(node['var'], i)
            final = final and evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool()
            scope = scope.parent
            if not final:
                break

        return Bool(final)

    elif type == 'exist':
        set = evaluate_node_from_scope(node['set'], scope, interpreter)
        
        if not isinstance(set, HezaSet):
            raise ValueError("Se esperaba un Conjunto para recorrer en foreach")
        
        final = False
        for i in set.values:

            scope = Scope(scope)
            scope.add_var(node['var'], i)
            final = final or evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool()
            scope = scope.parent
            if final:
                break

        return Bool(final)

    elif type == 'summation':
        set = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()

        final = Null()
        for i in set.values:

            scope = Scope(scope)
            scope.add_var(node['var'], i)

            final += evaluate_node_from_scope(node['expresion'], scope, interpreter)

            scope = scope.parent

        return final

    elif type == 'production':
        set = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()

        final = Number(1)
        for i in set.values:

            scope = Scope(scope)
            scope.add_var(node['var'], i)

            final *= evaluate_node_from_scope(node['expresion'], scope, interpreter)

            scope = scope.parent

        return final

    elif type == 'filter':
        set = evaluate_node_from_scope(node['set'], scope, interpreter).to_set()

        filtered_set = HezaSet({})

        scope = Scope(scope)

        for i in set.values:

            scope = Scope(scope)
            scope.add_var(node['var'], i)

            if evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool():
                filtered_set.add(i)

            scope = scope.parent

        return filtered_set
    
    elif type == 'transformation':
    
        sets = [evaluate_node_from_scope(set, scope, interpreter).to_set() for set in node['sets']]

        final_set = HezaSet({})

        for i in range(int(sets[0].to_abs())):

            scope = Scope(scope)

            for var, set in zip(node['vars'], sets):
                scope.add_var(var, set.get(Number(i)))

            if evaluate_node_from_scope(node['condition'], scope, interpreter).to_bool():
                final_set.add(evaluate_node_from_scope(node['expresion'], scope, interpreter))

            scope = scope.parent

        return final_set
    
    elif type == 'lim':
        
        expresion = evaluate_node_from_scope(node['expresion'], scope, interpreter).to_expresion()

        lim = expresion.limit(node['var'], node['value'], node['dir'])

        return lim

    else:
        return Null()

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

    final_range = HezaSet({})

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

def convertions(value, type):
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

    if type['type'] != 'id':
        raise ValueError("Se esperaba un identificador como tipo futuro")
    
    tipo = type['value']

    if tipo not in convertions_allowed:
        raise ValueError(f"El tipo '{tipo}' no es un tipo permitido para conversion")
    
    if tipo == 'Number':
        return value.to_number()

    elif tipo == 'Text':
        return value.to_text()

    elif tipo == 'Expresion':
        return value.to_expresion()
    
    elif tipo == 'Set':
        return value.to_set()
    
    elif tipo == 'Inf':
        return value.to_infinite()
    
    elif tipo == 'Nah':
        return value.to_indeterminate()
    
    elif tipo == 'Null':
        return value.to_null()
        
    elif tipo == 'Bool':
        return value.to_bool()

def constantText(value):
    if value == 'ESP':
        return Text(" ")

    elif value == 'LINE':
        return Text("\n")

    return Text("\t")

def binary_operation(left, right, operation):

    if operation == 'add':
        return left + right
    
    if operation == 'sub':
        return left - right

    if operation == 'div':
        return left / right
    
    if operation == 'mod':
        return left % right

    if operation == 'mul':
        return left * right

    if operation == 'pow':
        return left ** right
    
    if operation == 'equal':
        return Bool(left == right)
    
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
        if not isinstance(right, HezaSet):
            raise ValueError("La expresion right del operador in debe ser un conjunto")
        return Bool(left in right.values)

    if operation == 'notin':
        if not isinstance(right, HezaSet):
            raise ValueError("La expresion right del operador not in debe ser un conjunto")
        return Bool(not (left in right.values))
    
    if operation == 'del':
        if not isinstance(left, HezaSet):
            raise ValueError("La expresion left del operador del debe ser un conjunto")
        return left.eliminar(right)

    if operation == 'then':

        return Bool((not left.to_bool()) or right.to_bool())

    if operation == 'and':
        return Bool(left.to_bool() and right.to_bool())

    if operation == 'or':
        return Bool(left.to_bool() or right.to_bool())
    
    if operation == 'union':
        if not isinstance(left, HezaSet):
            raise TypeError("El operando izquierdo debe ser un conjunto para union")
        return left.union(right)
    
    if operation == 'inter':
        if not isinstance(left, HezaSet):
            raise TypeError("El operando izquierdo debe ser un conjunto para interseccion")
        return left.inter(right)
    
    if operation == 'symDiff':
        if not isinstance(left, HezaSet):
            raise TypeError("El operando izquierdo debe ser un conjunto para diferencia simetrica")
        return left.symetric_difference(right)
    
    if operation == 'cross':
        if not isinstance(left, HezaSet):
            raise TypeError("El operando izquierdo debe ser un conjunto para producto cartesiano")
        return left.cross(right)

def unary_operation(value, operation):

    if operation == 'neg':
        return -value

    if operation == 'pos':
        return value

    if operation == 'fact':
        if not isinstance(value, Number):
            raise ValueError("La operacion fact(!) solo soporta valores de tipo Number")

        from math import factorial
        return Number(factorial(value.value))

    if operation == 'sqrt':
        if not isinstance(value, Number):
            raise ValueError("La operacion sqrt(√) solo soporta valores de tipo Number")

        from math import sqrt
        return Number(sqrt(value.value))

    if operation == 'not':
        return Bool(not value.to_bool())