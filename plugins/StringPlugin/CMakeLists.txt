cmake_minimum_required(VERSION 3.21)
project(StringPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_IMPORTS
)


add_library(
        StringPlugin
        SHARED
        stringLibrary.cpp
        )


target_include_directories(
        StringPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        StringPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        )


target_link_libraries(
        StringPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)
