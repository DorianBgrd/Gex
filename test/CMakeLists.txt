cmake_minimum_required(VERSION 3.25)
project(Test)

set(CMAKE_CXX_STANDARD 17)


add_executable(
        Test
        main.cpp
)


target_include_directories(
        Test
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        $ENV{TSYS_DIR}
)


target_link_directories(
        Test
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${TSYS_LIBRARY_DIR}
)


target_link_libraries(
        Test
        PUBLIC
        Gex
)


add_custom_command(
        TARGET
        Test

        POST_BUILD
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_executable.py
        --executable Test.exe --name Test --depth 2
        --directory ${CMAKE_CURRENT_BINARY_DIR}
        --search_directories ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TSYS_LIBRARY_DIR}
        --dependencies TSys.dll Gex.dll
)