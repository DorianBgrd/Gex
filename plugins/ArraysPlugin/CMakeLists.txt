cmake_minimum_required(VERSION 3.21)
project(ArraysPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        ArraysPlugin
        SHARED
        arraysLibrary.cpp)


target_include_directories(
        ArraysPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${PipelineLibrary_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        ArraysPlugin
        PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        ArraysPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
)


add_custom_command(
        TARGET
        ArraysPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name ArraysPlugin
        --plugin ArraysPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/ArraysPlugin.dll
        --type C++
)