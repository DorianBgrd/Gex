cmake_minimum_required(VERSION 3.21)
project(ConditionPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        ConditionPlugin
        SHARED
        condition.cpp
        condition.h
        plugin.cpp
)


target_include_directories(
        ConditionPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        ConditionPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Python_LIBRARY_DIRS}
        ${INTERNAL_LIBRARY_DIRS}
)


target_link_libraries(
        ConditionPlugin
        PUBLIC
        Gex
        Python::Python
        Python::Module
)


add_custom_command(
        TARGET
        ConditionPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/Gex_plugins/build_plugin.py

        --package
        --name ConditionPlugin
        --plugin ConditionPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/Gex_plugins/ConditionPlugin.dll
        --type C++
)