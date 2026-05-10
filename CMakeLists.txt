cmake_minimum_required(VERSION 3.24)
project(Gex)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DGEX_API_EXPORT)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

include(Third-parties.cmake)

set(
        INTERNAL_LIBRARY_DIRS
        ${CMAKE_BINARY_DIR}/Tsys
        ${CMAKE_BINARY_DIR}/UiTsys
        ${CMAKE_BINARY_DIR}/Gex
        ${CMAKE_BINARY_DIR}/Gex/python
        ${CMAKE_BINARY_DIR}/Gex_ui
        ${CMAKE_BINARY_DIR}/Gex_app
        ${CMAKE_BINARY_DIR}/Gex_plugins
)


add_subdirectory(Tsys)

add_subdirectory(UiRes)

add_subdirectory(UiTsys)

add_subdirectory(Gex)

add_subdirectory(Gex/python)

add_subdirectory(Gex_plugins)

add_subdirectory(cli)

add_subdirectory(test)



add_dependencies(
        Gex
        Tsys
)


add_dependencies(
        Gex_Python
        Gex
)



add_dependencies(
        ArraysPlugin
        Gex
)


#add_dependencies(
#        InputRel
#        Gex
#)





add_dependencies(
        Gex-cli
        Gex
)


add_subdirectory(Gex_ui)
add_subdirectory(
        Gex_app
)


add_dependencies(
        Gex_App
        App
)


add_dependencies(
        Gex_App
        Gex_ui
        App
)



add_dependencies(
        Gex_App
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
if (${Build_UsdPlugin})
    add_dependencies(
            Gex_App
            UsdPlugin
    )
endif()



add_dependencies(
        Test
        Gex
        Gex_App
)

add_dependencies(
        Test-deploy
        Test
)