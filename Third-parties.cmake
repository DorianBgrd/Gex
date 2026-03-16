message("SEARCH PATHS : ${CMAKE_MODULE_PATH}")

set(BOOST_DIR $ENV{BOOST_DIR})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_DEBUG_PYTHON ON)
set(Boost_DEBUG ON)

if(DEFINED Python_USE_VERSION)
    set(Boost_Python_VERSION ${Python_USE_VERSION})
else()
    set(Boost_Python_VERSION 311)
endif()

message("PYTHON VERSION : ${Boost_Python_VERSION}")

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

message("Python boost module : python${Boost_Python_VERSION}")

find_package(Boost 1.82.0 COMPONENTS python${Boost_Python_VERSION} REQUIRED)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

if (DEFINED Python_USE_VERSION)
    set(BUILD_SUFFIX -python${Python_USE_VERSION})
endif()


include("$ENV{TSYS_DIR}/tsys/TsysConfig.cmake")