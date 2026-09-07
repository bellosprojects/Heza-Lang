from src.engine.HSCE import *

def test_var_x():

    x = make_symbol('x')

    assert x.print() == 'x'