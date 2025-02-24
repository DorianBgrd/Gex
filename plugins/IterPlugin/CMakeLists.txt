cmake_minimum_required(VERSION 3.21)
project(IterPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
)


add_library(
        IterPlugin
        SHARED
        iter.cpp
)


target_include_directories(
        IterPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        IterPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        IterPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
)


add_custom_command(
        TARGET
        IterPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name IterPlugin
        --plugin IterPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/IterPlugin.dll
        --type C++
)