cmake_minimum_required(VERSION 3.24)
project(Gex)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DGEX_API_EXPORT)

set(BOOST_ROOT $ENV{BOOST_ROOT})

set(Boost_USE_STATIC_LIBS OFF)

find_package(Python 3.11 REQUIRED COMPONENTS Development Interpreter)
find_package(Boost 1.82.0 COMPONENTS python311 REQUIRED HINTS $ENV{BOOST_ROOT})

message("Python found ${Python_FOUND}")
message("Boost found ${Boost_FOUND}")

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(TSYS_LIBRARY_DIR "$ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}")

message("TSYS_LIBRARY_DIR : ${TSYS_LIBRARY_DIR}")


add_subdirectory(Gex)


add_subdirectory(Gex/python)


add_dependencies(
        Gex_Python
        Gex
)


add_subdirectory(plugins)


add_dependencies(
        ArraysPlugin
        Gex
)


#add_dependencies(
#        InputRel
#        Gex
#)


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
        Gex_Software
        Gex
        Gex_ui
        ArraysPlugin
        DebugPlugin
        FilesPlugin
#        InputRel
        IterPlugin
        MathPlugin
        PathsPlugin
        StringPlugin
)

#if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
add_dependencies(
        Gex_Software
        UsdPlugin
)
#endif()