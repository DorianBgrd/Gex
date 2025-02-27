cmake_minimum_required(VERSION 3.21)
project(JsonPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_library(
        JsonPlugin
        SHARED
        Plugin.cpp
        jsonDefs.h
        types/types.h
        types/types.cpp
        nodes/writers.h
        nodes/writers.cpp
        editor.h
        editor.cpp
)


target_include_directories(
        JsonPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        JsonPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        JsonPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        ${PYTHON_LINK_LIBRARIES}
)


add_custom_command(
        TARGET
        JsonPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name JsonPlugin
        --plugin JsonPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/JsonPlugin.dll
        --type C++
)