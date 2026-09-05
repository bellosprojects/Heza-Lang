import sympy as sp
from typing import Dict

class Relations:

    """
    Relaciones Algebraicas
    ---
    """

    def __init__(self):
        self.symbols : Dict[str, sp.Symbol] = {}
        self.dependences : Dict[str, sp.Expr] = {}
        self.values : Dict[str, float] = {}

    def _add_var(self, var : str):

        if var not in self.symbols:
            self.symbols[var] = sp.Symbol(var)

    def add_dependence(self, var : str, expr : sp.Expr):

        if var in self.dependences:

            eq = sp.Eq(self.dependences[var], expr)

            for s in eq.free_symbols:

                self.set_value(str(s), sp.solve(eq, s))

        self.dependences[var] = expr

        self._add_var(var=var)

        if isinstance(expr, sp.Expr):
            for sym in expr.free_symbols:
                self._add_var(str(sym))

    def get_value(self, var : str) -> sp.Expr:

        return self.values.get(var)
    
    def set_value(self, var : str, value : float):

        self.values[var] = value
    
rel = Relations()

x = sp.Symbol('x')
y = sp.Symbol('y')

rel.add_dependence('y', x + 4)

rel.add_dependence('y', 0)

print(rel.get_value('x'))