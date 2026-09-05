from setuptools import setup
from pathlib import Path
from pybind11.setup_helpers import Pybind11Extension, build_ext

BASE_DIR = Path(__file__).parent.resolve()

archivos_fuente = [str(p) for p in (BASE_DIR / "src").glob("*.cpp")]

# Define la extensión. El primer argumento es el nombre del módulo en Python.
ext_modules = [
    Pybind11Extension(
        "HSCE",
        archivos_fuente,
        include_dirs=[str(BASE_DIR / "include")]
    )
]

setup(
    name="HSCE",
    version="1.0.0",
    description="Motor de cálculo simbólico en C++ para Heza",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)