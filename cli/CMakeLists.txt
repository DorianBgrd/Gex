cmake_minimum_required(VERSION 3.24)
project(Gex-cli)

set(CMAKE_CXX_STANDARD 17)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

set(TSYS_LIBRARY_DIR "$ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}")

add_executable(
        Gex-cli
        src/main.cpp
        include/exec.h
        src/exec.cpp
        src/exec.cpp)


message("ARG PARSE DIR $ENV{ARGPARSE_DIR}")

target_include_directories(
        Gex-cli
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        $ENV{RAPIDJSON_DIR}
        $ENV{TSYS_DIR}
        $ENV{SAFEPTRS_DIR}
        $ENV{ARGPARSE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ARGPARSE_DIR}
        ${CMAKE_SOURCE_DIR}
)

target_link_directories(
        Gex-cli
        PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${TSYS_LIBRARY_DIR}
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(
        Gex-cli
        PUBLIC
        Gex
        TSys
)


add_custom_target(
        Gex-cli-build
        ALL
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_executable.py
        --executable Gex-cli.exe --name Gex-cli --depth 2
        --directory ${CMAKE_CURRENT_BINARY_DIR}
        --search_directories ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TSYS_LIBRARY_DIR}
        ${CMAKE_BINARY_DIR}/Gex
        --dependencies TSys.dll Gex.dll
        DEPENDS
        Gex-cli
)