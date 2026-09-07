from HSCE import CAS

def test_func_par():

    x = CAS.make_symbol('x')

    assert x.print() == 'x'