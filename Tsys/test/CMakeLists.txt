cmake_minimum_required(VERSION 3.24)

project(TypeSystem_test)


add_executable(
        TypeSystem_test

        test.cpp
)


target_include_directories(
        TypeSystem_test
        PUBLIC
        ${CMAKE_SOURCE_DIR}/Tsys/include
        $ENV{RAPIDJSON_DIR}
        ${Python_INCLUDE_DIRS}
        ${pybind11_INCLUDE_DIRS}
)


target_link_directories(
        TypeSystem_test
        PUBLIC
        ${INTERNAL_LIBRARY_DIRS}
)

target_link_libraries(
        TypeSystem_test
        PUBLIC
        TSys
        ${Python_LINK_LIBRARIES}
)


add_custom_target(
        TypeSystem_test-deploy
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name TypeSystem_test
        --executable ${CMAKE_CURRENT_BINARY_DIR}/TypeSystem_test.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

        --library-search-dir ${Python_LIBRARY_DIRS}
        --library-search-dir ${CMAKE_CURRENT_SOURCE_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}/plugins
        --library-search-dir ${CMAKE_BINARY_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}/Tsys

        --library Tsys.dll
        --library ${Python_LINK_LIBRARIES}

        DEPENDS
        TypeSystem_test
)

add_dependencies(
        TypeSystem_test
        Tsys
)