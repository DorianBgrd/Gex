cmake_minimum_required(VERSION 3.21)
project(ImageManipPlugin CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")

set(CMAKE_CXX_FLAGS "/EHsc")


add_definitions(
        /DPipelineLibrary2_EXPORT
        /DQT_NO_KEYWORDS
)


set(
        ImageManip_Core_Sources

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
        Types/geometry.cpp

        Nodes/blur.cpp
        Nodes/load.cpp
        Nodes/blend.cpp
        Nodes/noises.cpp
        Nodes/remap.cpp
        Nodes/shapes.cpp
        Nodes/level.cpp
        Nodes/tile.cpp
        Nodes/channels.cpp
)


set(
        ImageManip_UI_Sources
        Ui/Types.cpp
        Ui/Viewers/Viewers.cpp

        Ui/uiplugin.cpp
)


add_library(
        ImageManipPlugin
        SHARED

        ${ImageManip_Core_Sources}
        ${ImageManip_UI_Sources}
        plugin.cpp
)


set(
        ImageManip_IncludeDirs
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


target_include_directories(
        ImageManipPlugin
        PUBLIC
        ${ImageManip_IncludeDirs}
)

set(
        ImageManip_LinkDirs

        ${CMAKE_BINARY_DIR}
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}

        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
)


target_link_directories(
        ImageManipPlugin
        PUBLIC
        ${ImageManip_LinkDirs}
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


set(Qt_Deploy_Executable "windeployqt.exe")

set(Qt_DeployFlag --release)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(Qt_DeployFlag --debug)
endif()


add_custom_command(
        TARGET
        ImageManipPlugin
        POST_BUILD
        COMMAND
        ${Qt_Deploy_Executable} ${Qt_DeployFlag}
        --dir ${CMAKE_CURRENT_BINARY_DIR}/ImageManipPlugin/libs
        ${CMAKE_BINARY_DIR}/plugins/ImageManipPlugin.dll

)


# Tests --------------------------
add_executable(
        ImageManipTest

        ${ImageManip_Core_Sources}
        test.cpp
)

target_include_directories(
        ImageManipTest
        PUBLIC
        ${ImageManip_IncludeDirs}
        ${ImageManip_LinkDirs}
)

target_link_directories(
        ImageManipTest
        PUBLIC
        ${ImageManip_LinkDirs}
)

target_link_libraries(
        ImageManipTest
        PUBLIC
        Gex
        TSys
        UiTSys
        Gex_ui
        UiRes
        ${Qt6Widgets_LIBRARIES}
)


add_custom_command(
        TARGET
        ImageManipTest
        POST_BUILD
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name ImageManipTest
        --executable ${CMAKE_BINARY_DIR}/plugins/ImageManipTest.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

#        --library-search-dir ${Boost_LIBRARY_DIRS}
        --library-search-dir ${Python_LIBRARY_DIRS}
        --library-search-dir ${CMAKE_CURRENT_SOURCE_DIR}
        --library-search-dir ${TSYS_LIBRARY_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}

        --library ${TSYS_LIBRARY_DIR}/TSys.dll
        --library ${CMAKE_BINARY_DIR}/Gex/Gex.dll

)