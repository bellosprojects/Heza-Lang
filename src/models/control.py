class Control:

    def __init__(self, type:str, value = None):
        if type not in ('stop', 'return'):
            raise ValueError("Error en Control")
        
        self.type = type
        self.value = value

    def __repr__(self):
        return f"Control({self.type, {self.value}})"

    def __hash__(self) -> int:
        return hash(self.type)