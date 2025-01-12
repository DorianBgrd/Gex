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
        Geom.h
        Geom.cpp

        Ui/UsdUiPlugin.cpp
#        Ui/Viewers.h
#        Ui/Viewers.cpp

        UsdPlugin.cpp
)

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


add_custom_command(
        TARGET
        UsdPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py
        --libs-search-path
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${Python_LIBRARY_DIRS}/..
        ${Boost_LIBRARY_DIRS}
        ${PXR_ROOT}/lib

        --libs
        python3.dll
        ${Qt6Widgets_LIBRARIES}
        ${Boost_LIBRARIES}

        TSys.dll
        UiTSys.dll
        Gex_ui.dll

        usd_arch.dll
        usd_tf.dll
        usd_gf.dll
        usd_pegtl.dll
        usd_js.dll
        usd_trace.dll
        usd_work.dll
        usd_plug.dll
        usd_vt.dll
        usd_ts.dll
        usd_ar.dll
        usd_kind.dll
        usd_sdf.dll
        usd_ndr.dll
        usd_sdr.dll
        usd_pcp.dll
        usd_usd.dll
        usd_usdGeom.dll
        usd_usdVol.dll
        usd_usdMedia.dll
        usd_usdShade.dll
        usd_usdLux.dll
        usd_usdProc.dll
        usd_usdRender.dll
        usd_usdHydra.dll
        usd_usdRi.dll
        usd_usdSkel.dll
        usd_usdUI.dll
        usd_usdUtils.dll
        usd_usdPhysics.dll
        usd_usdMtlx.dll
        usd_garch.dll
        usd_hf.dll
        usd_hio.dll
        usd_cameraUtil.dll
        usd_pxOsd.dll
        usd_geomUtil.dll
        usd_glf.dll
        usd_hgi.dll
        usd_hgiGL.dll
        usd_hgiInterop.dll
        usd_hd.dll
        usd_hdar.dll
        usd_hdGp.dll
        usd_hdsi.dll
        usd_hdMtlx.dll
        usd_hdSt.dll
        usd_hdx.dll
        usd_usdImaging.dll
        usd_usdImagingGL.dll
        usd_usdProcImaging.dll
        usd_usdRiPxrImaging.dll
        usd_usdSkelImaging.dll
        usd_usdVolImaging.dll
        usd_usdAppUtils.dll
        usd_usdviewq.dll
        usd_usdBakeMtlx.dll


        --env
        PATH:./libs

        --python-path
        ./scripts

        --scripts
        ${PXR_ROOT}/lib/python/pxr

        --package
        --name UsdPlugin
        --plugin UsdPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/UsdPlugin.dll
        --type C++
)


set(Qt_Deploy_Executable "windeployqt.exe")


set(Qt_DeployFlag --release)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(Qt_DeployFlag --debug)
endif()


add_custom_command(
        TARGET
        UsdPlugin
        POST_BUILD
        COMMAND
        ${Qt_Deploy_Executable} ${Qt_DeployFlag}
        --libdir ${CMAKE_CURRENT_BINARY_DIR}/UsdPlugin/libs
        ${CMAKE_BINARY_DIR}/plugins/UsdPlugin.dll
)
