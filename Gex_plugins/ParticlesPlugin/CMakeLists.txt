cmake_minimum_required(VERSION 3.21)
project(ParticlesPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
)


add_library(
        ParticlesPlugin
        SHARED
        System.h
        System.cpp
        Particles.cpp
        Solver.cpp
        Field.cpp

        fields/Spin.h
        fields/Spin.cpp
        )


target_include_directories(
        ParticlesPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
)


target_link_directories(
        ParticlesPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Python_LIBRARY_DIRS}
        ${INTERNAL_LIBRARY_DIRS}
)


target_link_libraries(
        ParticlesPlugin
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
)
