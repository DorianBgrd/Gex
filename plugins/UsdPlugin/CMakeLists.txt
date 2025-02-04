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
        /DTBB_USE_DEBUG=0
)

set(PXR_DIR "$ENV{PXR_ROOT}-${CMAKE_BUILD_TYPE}")

find_package(pxr COMPONENTS Usd UsdGeom Tf REQUIRED HINTS PXR_DIR)


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

set(PXR_LIBRARY_DIR "$ENV{PXR_ROOT}-${CMAKE_BUILD_TYPE}/lib")


target_link_directories(
        UsdPlugin
        PUBLIC

        ${Boost_LIBRARY_DIRS}
        ${PipelineLibrary_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${PXR_LIBRARY_DIR}

        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
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


# ------------------------------------------
# Prepare a list of dlls to be used in the
# plugin packaging command.
set(PXR_DLLS "")

FOREACH(lib ${PXR_LIBRARIES})

    LIST(APPEND PXR_DLLS "usd_${lib}.dll")

endforeach()
# ------------------------------------------


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
        ${PXR_LIBRARY_DIR}

        --libs
        python3.dll
        ${Boost_LIBRARIES}

        TSys.dll
        UiTSys.dll
        Gex_ui.dll

        ${PXR_DLLS}

        --env
        PATH:./libs

        --python-path
        ./scripts

        --scripts
        ${PXR_LIBRARY_DIR}/python/pxr

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
