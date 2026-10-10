import src.engine.HSCE as cas

def test_aritmethics():

    x1 = cas.Number(4)
    x2 = cas.Number(9)
    x3 = cas.Number(6)
    x4 = cas.Number(8)
    x5 = cas.Number(3)

    assert (x1 + x2 + -(x3 + x4) * x5).is_equal(cas.Number(-29))

def test_vars_simplified():

    x = cas.Variable("x")
    y = cas.Variable("y")
    z = cas.Variable("z")

    expr1 = x * y + z ** cas.Number(2)
    expr2 = x * y * cas.Number(6)

    assert (expr1 + expr2 + - z ** cas.Number(2)).simplify().is_equal(x * y * cas.Number(7))

if __name__ == "__main__":
    test_aritmethics()