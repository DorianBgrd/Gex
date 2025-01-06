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
        InputRelPlugin
        SHARED
        Func.h
        Func.cpp
        Nodes.h
        Nodes.cpp
#        BezierFunc.h
#        BezierFunc.cpp

        Plugin.cpp

        # Ui files.
        viewer/main.cpp

        viewer/LinearFunc.h
        viewer/LinearFunc.cpp

        viewer/FuncViewer.h
        viewer/FuncViewer.cpp
#        ui/BezierFunc.h
#        ui/BezierFunc.cpp
        viewer/Viewer.h
        viewer/Viewer.cpp)


target_include_directories(
        InputRelPlugin
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
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
        InputRelPlugin
        PUBLIC
        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}

        ${GEX_LINK_DIRECTORIES}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
)


target_link_libraries(
        InputRelPlugin
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


add_custom_command(
        TARGET
        InputRelPlugin
        POST_BUILD
        COMMAND
        python ${CMAKE_SOURCE_DIR}/plugins/build_plugin.py

        --package
        --name InputRelPlugin
        --plugin InputRelPlugin.dll
        --plugin-file ${CMAKE_BINARY_DIR}/plugins/InputRelPlugin.dll
        --type C++
        --env PATH:./libs

        --libs-search-path
        ${CMAKE_BINARY_DIR}/ui
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}

        --libs
        Gex_ui.dll
        UiRes.dll
        UiTSys.dll
)
