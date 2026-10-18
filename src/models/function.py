import engine.HSCE as cas

class Function:

    def __init__(self, args: list[str], expresions: list[dict]):
        self.args = args
        self.expresions = expresions
            
    def __repr__(self):
        return f"[Function]"
class HezaFunction:
    """Objeto invocable para funciones definidas con 'function' (a trozos)."""
    def __init__(self, func_def, closure):
        self.func_def : Function = func_def
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

    def to_bool(self):

        from .bool import Bool
        return Bool(True)

    def print(self):
        return "Fun(" + ", ".join(self.func_def.args) + ")"