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
        src/Attribute.cpp
        src/Evaluation.cpp
        src/Graph.cpp
        src/Node.cpp
        src/NodeAttributeData.cpp
        src/NodeFactory.cpp
        src/PluginLoader.cpp
        include/Config.h
        src/Config.cpp
        include/Status.h
        include/Gex.h
        include/io.h
        src/io.cpp
        include/Profiler.h
        src/Profiler.cpp)


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