cmake_minimum_required(VERSION 3.15)
project(HSCE)

# Buscar Python y pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Crear el módulo de extensión
pybind11_add_module(HSCE engine.cpp eq.cpp expr.cpp)

# Especificar estándar C++17
target_compile_features(HSCE PRIVATE cxx_std_17)

# (Opcional) Para MSVC, evitar warnings de seguridad
if(MSVC)
    target_compile_options(HSCE PRIVATE /W4 /wd4100 /wd4127)
endif()

# Instalar el módulo
install(TARGETS HSCE DESTINATION ${Python_SITELIB})