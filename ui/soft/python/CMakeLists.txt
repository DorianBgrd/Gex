cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SoftPython CXX)

set(PROJECT_NAME SoftPython)


add_definitions(/DSOFT_PYTHON_API_EXPORT)


add_library(
        SoftPython
        SHARED
        module.cpp
        include/output.h
        src/output.cpp
)


target_link_libraries(
        SoftPython
        PUBLIC
        ${Boost_LIBRARIES}
        Gex_ui
)

target_include_directories(
        SoftPython
        PUBLIC
        ${Boost_INCLUDE_DIRS}
        $ENV{GEX_ROOT}
)



