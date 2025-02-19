cmake_minimum_required(VERSION 3.24)
project(Gex)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DGEX_API_EXPORT)

set(BOOST_DIR $ENV{BOOST_DIR})

set(Boost_USE_STATIC_LIBS OFF)

set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

find_package(Python 3.11 REQUIRED COMPONENTS Development Interpreter)

# Due to boost build not using python debug by default, and
# sometimes not linking it correctly when built from source
# (linking to both python libs) it is disabled by default and
# only release version of python is linked.
if (DEFINED Python_DEBUG)
    message(STATUS "Python_DEBUG variable is set to ${Python_DEBUG}")
    set(Impl_Python_DEBUG ${Python_DEBUG})
elseif (DEFINED ENV{Python_DEBUG})
    message(STATUS "Python_DEBUG variable is set to ${Python_DEBUG} (From environment)")
    set(Impl_Python_DEBUG $ENV{Python_DEBUG})
else()
    set(Impl_Python_DEBUG ON)
endif()

if (NOT ${Impl_Python_DEBUG})
    set(Python_LINK_LIBRARIES ${Python_LIBRARY_RELEASE})
    message(STATUS "Forced using python release libraries")
else()
    set(Python_LINK_LIBRARIES ${Python_LIBRARIES})
endif()

find_package(Boost 1.82.0 COMPONENTS python311 REQUIRED)


string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)


set(TSYS_LIBRARY_DIR "$ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}")


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