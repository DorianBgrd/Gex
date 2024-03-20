cmake_minimum_required(VERSION 3.24)
project(Gex)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DGEX_PYTHON_API_EXPORT)


#find_package(Python 3.10 REQUIRED COMPONENTS Development Interpreter)
#find_package(Boost 1.77.0 COMPONENTS python310 REQUIRED)


add_library(
        Gex_Python
        SHARED
        src/Attribute.cpp
        src/Graph.cpp
        src/Node.cpp
        src/import.cpp
        include/PluginLoader.h
        src/PluginLoader.cpp
        include/io.h
        src/io.cpp
        include/status.h
        src/status.cpp)


target_include_directories(
        Gex_Python
        PUBLIC
        ${CMAKE_SOURCE_DIR}
)

target_link_directories(
        Gex_Python
        PUBLIC
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(
        Gex_Python
        PUBLIC
        Gex
)


set(BUILD_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
        TARGET
        Gex_Python

        POST_BUILD
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_package.py --directory ${BUILD_PACKAGE_DIR}
        --name Gex --search_directories ${BUILD_PACKAGE_DEPENDENCIES_DIRS} ${Boost_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TSYS_LIBRARY_DIR}
        --library ${CMAKE_CURRENT_BINARY_DIR}/Gex_Python.dll
        --dependencies Gex.dll
)