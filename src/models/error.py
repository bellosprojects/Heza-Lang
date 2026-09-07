class TypeError(Exception):
    
    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return f"[Error de Tipo]: {self.msg}"