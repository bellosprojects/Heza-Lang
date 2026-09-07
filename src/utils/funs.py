# utils/funs.py
import math
import random
import keyboard

def rand(a, b):
    from src.models import Number
    return Number(random.randint(int(a.value), int(b.value)))

def cos(x):
    from src.models import Number
    return Number(math.cos(float(x.value)))

def sin(x):
    from src.models import Number
    return Number(math.sin(float(x.value)))

def tan(x):
    from src.models import Number
    return Number(math.tan(float(x.value)))

def ln(x):
    from src.models import Number
    return Number(math.log(float(x.value)))

def exp(x):
    from src.models import Number
    return Number(math.exp(float(x.value)))

def pressed(key):
    from src.models import Bool
    return Bool(keyboard.is_pressed(str(key.value)))

def sleep(ms):
    import time
    time.sleep(float(ms.value) / 1000.0)
    from src.models import Null
    return Null()