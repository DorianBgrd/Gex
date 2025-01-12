cmake_minimum_required(VERSION 3.21)
project(InputRel CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        InputRel_Python
        SHARED
        wrapper.cpp
        module.cpp
)


target_include_directories(
        InputRel_Python
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}

        ${CMAKE_SOURCE_DIR}/plugins/InputRel
)


target_link_directories(
        InputRel_Python
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)


target_link_directories(
        InputRel_Python
        PUBLIC
        ${CMAKE_BINARY_DIR}/plugins/InputRelPlugin/InputRelPlugin
)


target_link_libraries(
        InputRel_Python
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
        InputRelPlugin
)


set(BUILD_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
        TARGET
        InputRel_Python

        POST_BUILD
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_package.py --directory ${BUILD_PACKAGE_DIR}
        --name InputRel --search_directories ${CMAKE_BINARY_DIR}/plugins ${BUILD_PACKAGE_DEPENDENCIES_DIRS} ${Boost_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TSYS_LIBRARY_DIR} ${CMAKE_BINARY_DIR}/Gex
        --library ${CMAKE_BINARY_DIR}/plugins/InputRel_Python.dll
        --dependencies Gex.dll InputRel.dll
)

