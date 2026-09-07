cmake_minimum_required(VERSION 3.15)
project(HSCE)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp")
file(GLOB_RECURSE PYTHON_SOURCES "python/*.cpp")

pybind11_add_module(HSCE ${CORE_SOURCES} ${PYTHON_SOURCES})

target_compile_features(HSCE PRIVATE cxx_std_17)

target_include_directories(HSCE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(HSCE PRIVATE /W4 /wd4100 /wd4127)
endif()

install(TARGETS HSCE DESTINATION ${Python_SITELIB})