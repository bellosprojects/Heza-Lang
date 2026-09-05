import sympy as sp

# Definimos las funciones SymPy y la cantidad de argumentos que esperan.
# Esto asegura la verificación de aridad (arity check).
SYMPY_FUNCTION_MAP = {
    'sin':   (sp.sin, 1),
    'cos':   (sp.cos, 1),
    'tan':   (sp.tan, 1),
    'ln':   (sp.log, 1),
    'exp':   (sp.exp, 1),
    'sqrt':  (sp.sqrt, 1),
    'abs':   (sp.Abs, 1),
    'atan':  (sp.atan, 1),
    'log': (sp.log, 2),
}

def heza_ast_to_sympy(ast):
    """
    Convierte el AST de Heza (una estructura dict) a un objeto SymPy.

    Args:
        ast (dict): AST de Heza

    Returns:
        sympy (SymPy): Objeto sympy evaluable

    Raises:
        TypeError: Nodo o token invalido (ej. dataDeclaration)
    """
    node_type = ast['type']

    if node_type == 'number':
        return sp.Number(ast['value'])
    
    if node_type == 'id':
        return sp.Symbol(ast['value'])

    if node_type == 'infinite':
        return sp.oo

    # --- Nodos de Operaciones Unarias ---
    elif node_type == 'unaryOperation':
        return _convert_unary_operation(ast)

    # --- Nodos de Operaciones Binarias ---
    elif node_type == 'binaryOperation':
        return _convert_binary_operation(ast)
    
    # --- Nodos de llamadas a funciones ---
    elif node_type == 'call':
        return _convert_call_operation(ast)
    
    # --- Nodo Abs |x| ---
    elif node_type == 'abs':
        return sp.Abs(heza_ast_to_sympy(ast['value']))

    # Si llega un nodo que no debería estar aquí (ej., 'dataDeclaration')
    else:
        raise TypeError(f"Tipo de nodo no válido para SymPy: {node_type}")
    
def _convert_unary_operation(ast):
    """Maneja los nodos de operación unaria."""

    value_sympy = heza_ast_to_sympy(ast['value'])
    operation = ast['operation']

    if operation == 'neg':
        return -value_sympy
    
    elif operation == 'pos':
        return +value_sympy
        
    elif operation == 'sqrt':
        return sp.sqrt(value_sympy)
        
    elif operation == 'fact':
        return sp.factorial(value_sympy) 
        
    else:
        raise TypeError(f"Operación unaria desconocida para SymPy: {operation}")

def _convert_binary_operation(ast):
    """Maneja los nodos de operación binaria aritmética."""
    
    left_sympy = heza_ast_to_sympy(ast['leftValue'])
    right_sympy = heza_ast_to_sympy(ast['rightValue'])
    
    op_map = {
        'add': sp.Add,    
        'sub': sp.Add,      
        'mul': sp.Mul,        
        'div': sp.Mul,        
        'pow': sp.Pow,        
        'mod': sp.Mod,
    }
    
    operation = ast['operation']

    if operation == 'sub':
        return left_sympy + (-right_sympy)
    
    elif operation == 'div':
        return left_sympy * sp.Pow(right_sympy, -1)
    
    elif operation in op_map:
        return op_map[operation](left_sympy, right_sympy)
    
    else:
        raise TypeError(f"Operación aritmética binaria desconocida: {operation}")
    
def _convert_call_operation(ast):
    """Maneja el nodo 'call' traduciendo funciones matemáticas a SymPy y verifica la aridad."""

    function_name = ast['name']
    args_list = ast['args']
    
    if function_name not in SYMPY_FUNCTION_MAP:
        # Si no es una función matemática SymPy, asumimos que es una función de usuario
        # y esta debe ser manejada por el Evaluator de Heza, no en la traducción simbólica.
        raise TypeError(f"Llamada a función no válida para cálculo simbólico: '{function_name}'.")

    # Obtener la función de SymPy y la aridad esperada
    sympy_func, expected_arity = SYMPY_FUNCTION_MAP[function_name]
    
    # 1. VERIFICACIÓN DE ARIDAD
    if len(args_list) != expected_arity:
        raise TypeError(f"La función '{function_name}' requiere {expected_arity} argumento(s), pero se encontraron {len(args_list)}.")

    sympy_args = [heza_ast_to_sympy(arg_node) for arg_node in args_list]
    
    return sympy_func(*sympy_args)

def sympy_to_heza(sympy: str):
    convertions = {
        'log': 'ln',
        'oo': 'c',
        '**': '^'
    }

    for c in convertions:
        sympy = sympy.replace(c, convertions[c])
    return sympy