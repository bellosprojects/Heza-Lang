class Function:

    def __init__(self, args, expresions):
        self.args = args
        self.expresions = expresions

    def get(self, scope):

        from utils import evaluate_node_from_scope

        final = None
        for i in self.expresions:
            if evaluate_node_from_scope(i['condition'], scope):
                return evaluate_node_from_scope(i['expresion'], scope)
            
    def __repr__(self):
        return f"[Function]"
class HezaFunction:
    """Objeto invocable para funciones definidas con 'function' (a trozos)."""
    def __init__(self, func_def, closure):
        self.func_def = func_def
        self.closure = closure

    def call(self, args, interpreter):
        if len(self.func_def.args) != len(args):
            raise ValueError("Número de parámetros incorrecto")
        
        from .scope import Scope
        from utils import evaluate_node_from_scope

        new_scope = Scope(self.closure)
        for name, val in zip(self.func_def.args, args):
            new_scope.add_var(name, val, in_this=True)
        for expr in self.func_def.expresions:
            cond = evaluate_node_from_scope(expr['condition'], new_scope, interpreter)
            if cond.to_bool():
                return evaluate_node_from_scope(expr['expresion'], new_scope, interpreter)
        return None