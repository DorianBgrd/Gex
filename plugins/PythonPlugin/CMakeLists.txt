cmake_minimum_required(VERSION 3.21)
project(PythonPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        PythonPlugin
        SHARED
        nodes.cpp
        expression.cpp
        Plugin.cpp
)


target_include_directories(
        PythonPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        PythonPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        PythonPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
)


add_custom_command(
        TARGET
        PythonPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name PythonPlugin
        --plugin PythonPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/PythonPlugin.dll
        --type C++
)
