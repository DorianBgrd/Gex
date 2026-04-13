message("SEARCH PATHS : ${CMAKE_MODULE_PATH}")

set(BOOST_DIR $ENV{BOOST_DIR})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_DEBUG_PYTHON OFF)
set(Boost_DEBUG ON)

if(DEFINED Python_USE_VERSION)
    set(Python_VERSION_NUMBER ${Python_USE_VERSION})
else()
    set(Python_VERSION_NUMBER 313)
endif()

message(STATUS "PYTHON VERSION : ${Python_VERSION_NUMBER}")

set(Boost_Python_VERSION ${Python_VERSION_NUMBER})

set(Python_ROOT_DIR $ENV{Python_ROOT_DIR})

find_package(Python REQUIRED COMPONENTS Development Interpreter)

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
    set(Impl_Python_DEBUG OFF)
endif()

if (NOT ${Impl_Python_DEBUG})
    set(Python_LINK_LIBRARIES ${Python_LIBRARY_RELEASE})
    #    set(Boost_USE_DEBUG_PYTHON OFF)

    message(STATUS "Forced using python release libraries")
else()
    set(Python_LINK_LIBRARIES ${Python_LIBRARIES})
endif()

message(STATUS "Python boost module : python${Boost_Python_VERSION}")

#find_package(Boost 1.82.0 COMPONENTS python${Boost_Python_VERSION} REQUIRED)
set(pybind11_INCLUDE_DIRS $ENV{PYBIND11_DIR}/pybind11/include)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

if (DEFINED Python_USE_VERSION)
    set(BUILD_SUFFIX -python${Python_USE_VERSION})
endif()


list(APPEND CMAKE_PREFIX_PATH "$ENV{TSYS_DIR}/Tsys/releases/Tsys-Python${Python_VERSION_NUMBER}")
list(APPEND CMAKE_PREFIX_PATH "$ENV{UI_TSYS_DIR}/UiTsys/releases/UiTsys-Python${Python_VERSION_NUMBER}")
list(APPEND CMAKE_PREFIX_PATH "$ENV{UI_RES_DIR}/UiRes/releases/UiRes")

find_package(Tsys REQUIRED)

find_package(Qt6 COMPONENTS Widgets Core Gui Svg SvgWidgets REQUIRED)

find_package(UiRes REQUIRED)

find_package(UiTsys REQUIRED)


# message(STATUS "Tsys include dirs (found : ${Tsys_FOUND}) : ${Tsys_LIBRARIES}")