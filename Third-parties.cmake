set(BOOST_DIR $ENV{BOOST_DIR})

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_DEBUG_PYTHON ON)
set(Boost_DEBUG ON)

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
    set(Impl_Python_DEBUG OFF)
endif()

if (NOT ${Impl_Python_DEBUG})
    set(Python_LINK_LIBRARIES ${Python_LIBRARY_RELEASE})
    #    set(Boost_USE_DEBUG_PYTHON OFF)

    message(STATUS "Forced using python release libraries")
else()
    set(Python_LINK_LIBRARIES ${Python_LIBRARIES})
endif()

find_package(Boost 1.82.0 COMPONENTS python311 REQUIRED)

message(STATUS "Boost libraries : ${Boost_LIBRARIES}")

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)


set(TSYS_LIBRARY_DIR "$ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}")
