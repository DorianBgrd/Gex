cmake_minimum_required(VERSION 3.21)
project(PathsPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
)


add_library(PathsPlugin
        SHARED
        pathsLibrary.cpp
        )


target_include_directories(
        PathsPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
)


target_link_directories(
        PathsPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
)


target_link_libraries(
        PathsPlugin
        PUBLIC
        Gex
)


add_custom_command(
        TARGET
        PathsPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name PathsPlugin
        --plugin PathsPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/PathsPlugin.dll
        --type C++
)
