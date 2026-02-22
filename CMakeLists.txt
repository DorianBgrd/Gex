cmake_minimum_required(VERSION 3.24)
project(Gex)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DGEX_API_EXPORT)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

include(Third-parties.cmake)

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