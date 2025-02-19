cmake_minimum_required(VERSION 3.3.24)
project(Test)

set(CMAKE_CXX_STANDARD 17)


add_executable(
        Test

        tests.h

        testSuite/TestNodes.h
        testSuite/TestNodes.cpp

        testSuite/TestAttributes.h
        testSuite/TestAttributes.cpp

        main.cpp
)


target_include_directories(
        Test
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        $ENV{TSYS_DIR}
        $ENV{RAPIDJSON_DIR}
        $ENV{SAFEPTRS_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)


target_link_directories(
        Test
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${TSYS_LIBRARY_DIR}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        Test
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
)


#add_custom_command(
#        TARGET
#        Test
#
#        POST_BUILD
#        COMMAND
#        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_executable.py
#        --executable Test.exe --name Test --depth 2
#        --directory ${CMAKE_CURRENT_BINARY_DIR}
#        --search_directories ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TSYS_LIBRARY_DIR}
#        --dependencies TSys.dll Gex.dll
#)


add_custom_command(
        TARGET
        Test

        POST_BUILD
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name Gex
        --executable ${CMAKE_CURRENT_BINARY_DIR}/Test.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

        --library-search-dir ${Python_LIBRARY_DIRS}
        --library-search-dir ${CMAKE_CURRENT_SOURCE_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}/plugins
        --library-search-dir ${TSYS_LIBRARY_DIR}
        --library-search-dir $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        --library-search-dir $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        --library-search-dir ${CMAKE_BINARY_DIR}

        --library ${CMAKE_BINARY_DIR}/Gex/Gex.dll
        --library ${TSYS_LIBRARY_DIR}/TSys.dll
)