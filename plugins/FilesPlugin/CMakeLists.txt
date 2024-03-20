cmake_minimum_required(VERSION 3.21)
project(FilesPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
)


add_library(
        FilesPlugin
        SHARED
        filesLibrary.cpp
        fileUtils.h
)


target_include_directories(
        FilesPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        FilesPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        FilesPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)
