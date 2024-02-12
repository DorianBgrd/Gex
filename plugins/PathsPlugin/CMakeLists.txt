cmake_minimum_required(VERSION 3.21)
project(PathsPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")


# remove default flags provided with CMake for MSVC
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

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
