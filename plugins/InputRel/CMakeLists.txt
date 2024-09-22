cmake_minimum_required(VERSION 3.21)
project(InputRel CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
        /DGEX_UI_API_IMPORT
)


add_library(
        InputRel
        SHARED
        Func.h
        Func.cpp
        Nodes.h
        Nodes.cpp
        BezierFunc.h
        BezierFunc.cpp

        Plugin.cpp

        # Ui files.
        ui/main.cpp
        ui/LinearFunc.h
        ui/LinearFunc.cpp
        ui/BezierFunc.h
        ui/BezierFunc.cpp
)


target_include_directories(
        InputRel
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}

        $ENV{UI_TSYS_DIR}

        # Ui include dirs.
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
)


target_link_directories(
        InputRel
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}

        ${GEX_LINK_DIRECTORIES}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
)


target_link_libraries(
        InputRel
        PUBLIC
        Gex
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module

        # Ui library dlls.
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        TSys
        UiTsys
        Gex_ui
)



add_subdirectory(app)

add_dependencies(
        App
        InputRel
)