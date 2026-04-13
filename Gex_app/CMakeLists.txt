cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Gex_App CXX)


add_definitions("/DQT_NO_KEYWORDS")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


# ------------------------------------------
# First build an api as part of a bundle
# for python compatibility. A first library
# is created here, and then its python
# wrapper will be built linking it.

add_library(
        App
        SHARED
        include/PythonOutput.h
        src/PythonOutput.cpp

        include/AppContext.h
        src/AppContext.cpp

        include/MainWindow.h
        src/MainWindow.cpp

        include/PythonInterpreter.h
        src/PythonInterpreter.cpp
)

target_include_directories(
        App
        PUBLIC
        $ENV{RAPIDJSON_DIR}
        $ENV{TSYS_DIR}
        $ENV{UI_RES_DIR}
        $ENV{UI_TSYS_DIR}
        $ENV{QT_AWESOME_DIR}
        $ENV{ARGPARSE_DIR}

        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}

        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6Svg_INCLUDE_DIRS}
        ${Qt6SvgWidgets_INCLUDE_DIRS}
)

target_link_directories(
        App
        PUBLIC
        ${Python_LIBRARY_DIRS}
)

target_link_libraries(
        App
        PUBLIC
        Gex
        Gex_ui

        ${Python_LINK_LIBRARIES}

        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::SvgWidgets
)

target_compile_definitions(
        App
        PRIVATE
        SOFT_API_EXPORT
)


# ------------------------------------------
# Build the software and link the library
# to it to provide access to it.

add_executable(
        Gex_App
        main.cpp
#        include/MainWindow.h
#        src/MainWindow.cpp
#        include/PythonInterpreter.h
#        src/PythonInterpreter.cpp
)


add_compile_definitions(
        Gex_App
        PRIVATE
        GEX_UI_API_IMPORT
)



target_include_directories(
        Gex_App
        PUBLIC
        $ENV{RAPIDJSON_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/ui
        ${CMAKE_SOURCE_DIR}/ui/soft
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
        Gex_App
        PUBLIC
        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        ${QT_AWESOME_LIBRARY_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/ui
)


target_link_libraries(
        Gex_App
        PUBLIC
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::SvgWidgets
        Tsys::Tsys
        UiTsys::UiTsys
        UiRes::UiRes
        Gex
        Gex_ui
        App
        ${Python_LINK_LIBRARIES}
)


set(Qt_Deploy_Executable "windeployqt.exe")

set(Qt_DeployFlag --release)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(Qt_DeployFlag --debug)
endif()


if (${Build_UsdPlugin})
    set(PXR_LIBRARY_DIR ${PXR_ROOT}/lib)

    set(USD_PLUGIN_RELOC --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/UsdPlugin/UsdPlugin
            --library-search-dir ${PXR_LIBRARY_DIR})
endif()


add_custom_target(
        Deploy
        ALL
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name Gex
        --executable ${CMAKE_CURRENT_BINARY_DIR}/Gex_App.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

#        --library-search-dir ${Boost_LIBRARY_DIRS}
        --library-search-dir ${Python_LIBRARY_DIRS}
        --library-search-dir ${Python_ROOT_DIR}
        --library-search-dir ${CMAKE_CURRENT_SOURCE_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}/plugins
        --library-search-dir ${UiRes_LIBRARY_DIRS}
        --library-search-dir ${Tsys_LIBRARY_DIRS}
        --library-search-dir ${CMAKE_BINARY_DIR}

        --library ${CMAKE_CURRENT_BINARY_DIR}/App.dll
        --library ${CMAKE_BINARY_DIR}/Gex_ui/Gex_Ui.dll
        --library ${Tsys_LIBRARY}
        --library ${UiRes_LIBRARY}
        --library ${UiTsys_LIBRARY}
        --library ${CMAKE_BINARY_DIR}/Gex/Gex.dll
        --library ${QT_AWESOME_LIBRARY_DIR}/QtAwesome.dll
        --library Python${Python_VERSION_NUMBER}.dll

        --extra-directory ui
        --extra-directory plugins
        --extra-directory python

        --add-extra-directory python ${CMAKE_BINARY_DIR}/Gex/python/Gex
#        --add-extra-directory python ${CMAKE_BINARY_DIR}/ui/SoftPython.pyd

        --add-extra-directory ui ${CMAKE_CURRENT_SOURCE_DIR}/resources/stylesheet.css

        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/ArraysPlugin/ArraysPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/DebugPlugin/DebugPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/FilesPlugin/FilesPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/IterPlugin/IterPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/MathPlugin/MathPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/MayaPlugin/MayaPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/PathsPlugin/PathsPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/StringPlugin/StringPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/ImageManipPlugin/ImageManipPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/InputRelPlugin/InputRelPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/JsonPlugin/JsonPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/PythonPlugin/PythonPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/GeometryPlugin/GeometryPlugin
        ${USD_PLUGIN_RELOC}
        #        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/InputRel.dll
        #        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/GraphPlugin/GraphPlugin.dll

        COMMAND
        ${Qt_Deploy_Executable} ${Qt_DeployFlag} ${CMAKE_CURRENT_BINARY_DIR}/Gex_App/bin/Gex_App.exe
        DEPENDS
        Gex_App
)