cmake_minimum_required(VERSION 3.25)
project(Gex)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DGEX_API_EXPORT)


find_package(Python 3.10 REQUIRED COMPONENTS Development Interpreter)
find_package(Boost 1.77.0 COMPONENTS python310 REQUIRED)

message("Python found ${Python_FOUND}")

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(TSYS_LIBRARY_DIR "$ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}")

message("TSYS_LIBRARY_DIR : ${TSYS_LIBRARY_DIR}")

add_library(
        Gex
        SHARED
        Gex/src/Attribute.cpp
        Gex/src/Evaluation.cpp
        Gex/src/Graph.cpp
        Gex/src/Node.cpp
        Gex/src/NodeAttributeData.cpp
        Gex/src/NodeFactory.cpp
        Gex/src/PluginLoader.cpp
        Gex/include/Config.h
        Gex/src/Config.cpp
        Gex/include/Status.h
        Gex/include/Gex.h
        Gex/include/io.h
        Gex/src/io.cpp
        Gex/include/Profiler.h
        Gex/src/Profiler.cpp
        Gex/include/utils.h
        Gex/src/utils.cpp
        Gex/include/defs.h)


target_include_directories(
        Gex
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        $ENV{RAPIDJSON_DIR}
        $ENV{TSYS_DIR}
        $ENV{SAFEPTRS_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)

target_link_directories(
        Gex
        PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${TSYS_LIBRARY_DIR}
)

target_link_libraries(
        Gex
        PUBLIC
        ${Boost_LIBRARIES}
        ${Python_LIBRARIES}
        TSys
)


add_subdirectory(python)


add_dependencies(
        Gex_Python
        Gex
)


add_subdirectory(plugins)


add_dependencies(
        ArraysPlugin
        Gex
)


add_subdirectory(test)


add_dependencies(
        Test
        Gex
)


add_subdirectory(
        cli
)


add_dependencies(
        Gex-cli
        Gex
)


add_subdirectory(ui)


add_dependencies(
        Gex_ui
        Gex
)