class Reference:
    
    def __init__(self, value):
        self.value = value

    def evaluate(self, scope, interpreter):

        from utils import evaluate_node_from_scope

        value = evaluate_node_from_scope(self.value, scope, interpreter)

        return value

    def __repr__(self):
        return f"Reference({self.value['type']})"

    def print(self, scope):

        return self.evaluate(scope, None).print(scope)

    def to_bool(self):

        from .bool import Bool
        return Bool(True)

    def __hash__(self) -> int:
        return hash("Ref" + str(self.value))