import src.engine.HSCE as cas

def test_var_x():

    x1 = cas.Number(4)
    x2 = cas.Number(9)
    x3 = cas.Number(6)
    x4 = cas.Number(8)
    x5 = cas.Number(3)

    set_ = cas.FiniteSet({x1,x2,x3,x4,x5})

    print(set_.to_str())

if __name__ == "__main__":
    test_var_x()