cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UsdPlugin CXX)

set(CMAKE_CXX_FLAGS "/EHsc")

add_definitions("/DQT_NO_KEYWORDS")

########################################################################################################################
# External dependencies.
########################################################################################################################

add_definitions(
        /DPlugins_EXPORT
)


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(PXR_ROOT $ENV{PXR_DEBUG_ROOT})
else()
    set(PXR_ROOT $ENV{PXR_ROOT})
endif()


message("PXR ROOT : ${PXR_ROOT}")
find_package(pxr COMPONENTS Usd UsdGeom Tf REQUIRED HINTS ${PXR_ROOT})


# Find Qt package -------------------------------------------------------------
#find_package(Qt6 COMPONENTS Widgets Core Gui Svg SvgWidgets REQUIRED)
#qt_standard_project_setup()

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
# -----------------------------------------------------------------------------

add_library(
        UsdPlugin
        SHARED

        options.h
        UsdPlugin.cpp
        Ui/UsdUiPlugin.cpp
        Geom.h
        Geom.cpp)

target_include_directories(
        UsdPlugin
        PUBLIC

        ${CMAKE_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${PipelineLibrary_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}
        ${PXR_INCLUDE_DIRS}
        ${PXR_ROOT}/include

        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6Svg_INCLUDE_DIRS}
        ${Qt6SvgWidgets_INCLUDE_DIRS}

        $ENV{TSYS_DIR}
        $ENV{UI_TSYS_DIR}
        $ENV{RAPIDJSON_DIR}
        $ENV{SAFEPTRS_DIR}
)


target_link_directories(
        UsdPlugin
        PUBLIC

        ${Boost_LIBRARY_DIRS}
        ${PipelineLibrary_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${PXR_ROOT}/lib

        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}

        arch
        tf
        gf
        pegtl
        js
        trace
        work
        plug
        vt
        ts
        ar
        kind
        sdf
        ndr
        sdr
        pcp
        usd
        usdGeom
        garch
        hf
        geomUtil
        glf
)


target_link_libraries(
        UsdPlugin
        PUBLIC
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
        ${PXR_LIBRARIES}
        ${Python_LIBRARIES}

        TSys
        UiTSys
        Gex
        Gex_ui

        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::SvgWidgets
)
