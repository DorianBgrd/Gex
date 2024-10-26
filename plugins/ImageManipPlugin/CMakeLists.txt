cmake_minimum_required(VERSION 3.21)
project(ImageManipPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
        /DQT_NO_KEYWORDS
)


add_library(
        ImageManipPlugin
        SHARED
        Manip/blur.cpp
        Manip/blend.cpp
        Manip/noises.cpp
        Manip/remap.cpp
        Manip/shapes.cpp
        Manip/level.cpp
        Manip/tile.cpp
        Manip/channels.cpp

        Types/image.cpp
        Types/color.cpp
        Types/gradient.cpp
        Types/resolution.cpp
        Types/level.cpp

        Nodes/blur.cpp
        Nodes/load.cpp
        Nodes/blend.cpp
        Nodes/noises.cpp
        Nodes/remap.cpp
        Nodes/shapes.cpp
        Nodes/level.cpp
        Nodes/tile.cpp
        Nodes/channels.cpp

        plugin.cpp

        Ui/Types.cpp
        Ui/Viewers/Viewers.cpp

        Ui/uiplugin.cpp)


target_include_directories(
        ImageManipPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${ThirdParties_INCLUDE_DIRS}

        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6Svg_INCLUDE_DIRS}
        ${Qt6SvgWidgets_INCLUDE_DIRS}

        $ENV{TSYS_DIR}
        $ENV{UI_TSYS_DIR}
        $ENV{UI_RES_DIR}
        $ENV{RAPIDJSON_DIR}
        $ENV{SAFEPTRS_DIR}
)


target_link_directories(
        ImageManipPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}

        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
)


target_link_libraries(
        ImageManipPlugin
        PUBLIC
        Gex
        TSys
        UiTSys
        Gex_ui
        UiRes
        ${Qt6Widgets_LIBRARIES}
        #        ${Boost_LIBRARIES}
        #        Python::Python
        #        Python::Module
)


add_custom_command(
        TARGET
        ImageManipPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name ImageManipPlugin
        --plugin ImageManipPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/ImageManipPlugin.dll
        --type C++
)