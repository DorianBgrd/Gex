cmake_minimum_required(VERSION 3.21)
project(MathPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        MathPlugin
        SHARED

        nodes.h
        types.h

        nodes.cpp
        types.cpp

        mathLibrary.cpp
        LocalApi.h)


target_include_directories(
        MathPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}

        $ENV{TSYS_DIR}

        $ENV{UI_RES_DIR}
)


target_link_directories(
        MathPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}

        $ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}

        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        )


target_link_libraries(
        MathPlugin
        PUBLIC
        Gex

        ${Python_LINK_LIBRARIES}

        Tsys
        UiRes
        Gex_ui
)


add_custom_command(
        TARGET
        MathPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/Gex_plugins/build_plugin.py

        --package
        --name MathPlugin
        --plugin MathPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/MathPlugin.dll
        --type C++
#        --required-plugins IterPlugin
)