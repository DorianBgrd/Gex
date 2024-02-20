cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Gex_Soft CXX)

set(PROJECT_NAME Gex_Soft)


add_executable(
        Gex_Software
        main.cpp
        include/MainWindow.h
        src/MainWindow.cpp
)


add_compile_definitions(Gex_Software PRIVATE GEX_UI_API_IMPORT)



target_include_directories(
        Gex_Software
        PUBLIC
        $ENV{RAPIDJSON_DIR}
        ${CMAKE_SOURCE_DIR}
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6Svg_INCLUDE_DIRS}
        ${Qt6SvgWidgets_INCLUDE_DIRS}
        $ENV{TSYS_DIR}
        $ENV{UI_RES_DIR}
        $ENV{UI_TSYS_DIR}
        $ENV{QT_AWESOME_DIR}
        $ENV{ARGPARSE_DIR}
)


target_link_directories(
        Gex_Software
        PUBLIC
        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        ${QT_AWESOME_LIBRARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/ui
)


target_link_libraries(
        Gex_Software
        PUBLIC
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::SvgWidgets
        TSys
        UiTSys
        UiRes
        Gex
        Gex_ui
)


#add_custom_command(
#        TARGET
#        Gex_Software
#
#        POST_BUILD
#        COMMAND
#        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_executable.py
#        --executable  ${CMAKE_BINARY_DIR}/ui/Gex_Software.exe --name Gex_Software --depth 2
#        --directory ${CMAKE_CURRENT_BINARY_DIR}
#        --search_directories ${TSYS_LIBRARY_DIR}  $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
#        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE} ${QT_AWESOME_LIBRARY_DIR} ${CMAKE_BINARY_DIR}
#        ${CMAKE_BINARY_DIR}/ui
#        --dependencies TSys.dll Gex.dll UiTSys.dll Gex_ui.dll UiRes.dll
#)


add_custom_command(
        TARGET
        Gex_Software

        POST_BUILD
        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name Gex
        --executable ${CMAKE_BINARY_DIR}/ui/Gex_Software.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

        --library-search-dir ${Boost_LIBRARY_DIRS}
        --library-search-dir ${Python_LIBRARY_DIRS}
        --library-search-dir ${CMAKE_CURRENT_SOURCE_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}/plugins
        --library-search-dir ${TSYS_LIBRARY_DIR}
        --library-search-dir $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        --library-search-dir $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        --library-search-dir ${CMAKE_BINARY_DIR}

        --library ${CMAKE_BINARY_DIR}/ui/Gex_Ui.dll
        --library ${TSYS_LIBRARY_DIR}/TSys.dll
        --library $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}/UiRes.dll
        --library $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}/UiTSys.dll
        --library ${CMAKE_BINARY_DIR}/Gex.dll
        --library ${QT_AWESOME_LIBRARY_DIR}/QtAwesome.dll

        --extra-directory ui
        --extra-directory plugins
        --extra-directory python

        --add-extra-directory ui ${CMAKE_CURRENT_SOURCE_DIR}/resources/stylesheet.css

        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/ArraysPlugin/ArraysPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/DebugPlugin/DebugPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/FilesPlugin/FilesPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/IterPlugin/IterPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/MathPlugin/MathPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/MayaPlugin/MayaPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/PathsPlugin/PathsPlugin.dll
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/StringPlugin/StringPlugin.dll
#        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/UsdPlugin/UsdPlugin.dll
)




set(Qt_Deploy_Executable "windeployqt.exe")



set(Qt_DeployFlag --release)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(Qt_DeployFlag --debug)
endif()

add_custom_command(
        TARGET
        Gex_Software

        POST_BUILD
        COMMAND
        ${Qt_Deploy_Executable} ${Qt_DeployFlag} ${CMAKE_CURRENT_BINARY_DIR}/Gex_Software/bin/Gex_Software.exe
)