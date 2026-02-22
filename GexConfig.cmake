include(${CMAKE_CURRENT_LIST_DIR}/Third-parties.cmake)


string(TOLOWER ${CMAKE_BUILD_TYPE} LIB_BUILD_TYPE)


set(
        GEX_THIRD_PARTIES_INCLUDES

        $ENV{RAPIDJSON_DIR}
        $ENV{TSYS_DIR}
        $ENV{SAFEPTRS_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)


set(
        GEX_THIRD_PARTIES_LIBRARY_DIRS

        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${TSYS_LIBRARY_DIR}
)


set(
        GEX_INCLUDE_DIRS

        ${CMAKE_CURRENT_LIST_DIR}
        ${GEX_THIRD_PARTIES_INCLUDES}
)


set(GEX_ROOT_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/cmake-build-${LIB_BUILD_TYPE})


set(
        GEX_LIBRARY_DIRS

        ${GEX_ROOT_LIBRARY_DIR}
        ${GEX_ROOT_LIBRARY_DIR}/Gex
        ${GEX_ROOT_LIBRARY_DIR}/Gex/python
#        ${GEX_ROOT_LIBRARY_DIR}/ui
        ${GEX_THIRD_PARTIES_LIBRARY_DIRS}
)
