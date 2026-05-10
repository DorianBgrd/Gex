cmake_minimum_required(VERSION 3.21)
project(MathPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        GeometryPlugin
        SHARED

        types.h
        nodes.h

        types.cpp
        nodes.cpp

        plugin.cpp
)


target_include_directories(
        GeometryPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}

        $ENV{TSYS_DIR}

        $ENV{UI_RES_DIR}

        ../../plugins
)


target_link_directories(
        GeometryPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Python_LIBRARY_DIRS}

        ${INTERNAL_LIBRARY_DIRS}
)


target_link_libraries(
        GeometryPlugin
        PUBLIC
        Tsys
        Gex
        MathPlugin

        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}

        UiRes
        Gex_ui
)


add_custom_command(
        TARGET
        GeometryPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/Gex_plugins/build_plugin.py

        --package
        --name GeometryPlugin
        --plugin GeometryPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/Gex_plugins/GeometryPlugin.dll
        --type C++
        --required-plugins MathPlugin
)