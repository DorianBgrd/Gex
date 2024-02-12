cmake_minimum_required(VERSION 3.21)
project(ArraysPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")


# remove default flags provided with CMake for MSVC
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
)


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
        Python::Python
        Python::Module
)
