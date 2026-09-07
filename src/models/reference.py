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
        return self.evaluate(scope)