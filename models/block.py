class Block:

    def __init__(self, args, body):
        self.args = args
        self.body = body

class HezaBlock:
    """Objeto invocable que envuelve un bloque definido con 'fun'."""
    def __init__(self, block_def, closure):
        self.block_def = block_def   # instancia de Block
        self.closure = closure       # Scope donde fue definido

    def call(self, args, interpreter):
        if len(self.block_def.args) != len(args):
            raise ValueError("Número de parámetros incorrecto")
        

        from .scope import Scope
        from .control import Control

        new_scope = Scope(self.closure)
        for name, val in zip(self.block_def.args, args):
            new_scope.add_var(name, val, in_this=True)
        old_scope = interpreter.scope
        interpreter.scope = new_scope
        try:
            for inst in self.block_def.body:
                result = interpreter.process(inst)
                if isinstance(result, Control):
                    if result.type == 'return':
                        return result.value
                    elif result.type == 'stop':
                        raise RuntimeError('stop fuera de bucle')
        finally:
            interpreter.scope = old_scope
        return None