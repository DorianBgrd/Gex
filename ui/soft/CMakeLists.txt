cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Gex_Soft CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


# ------------------------------------------
# First build an api as part of a bundle
# for python compatibility. A first library
# is created here, and then its python
# wrapper will be built linking it.

add_library(
        SoftApi
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
        SoftApi
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
        SoftApi
        PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)

target_link_libraries(
        SoftApi
        PUBLIC
        Gex
        Gex_ui

        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}

        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::SvgWidgets
)

target_compile_definitions(
        SoftApi
        PUBLIC
        -DSOFT_API_EXPORT
)


# ------------------------------------------
# Build the python library component.

add_library(
        SoftPython
        SHARED
        src/module.cpp
)

target_compile_definitions(
        SoftPython
        PUBLIC
        -DSOFT_API_IMPORT
)

target_include_directories(
        SoftPython
        PUBLIC
        $ENV{RAPIDJSON_DIR}
        $ENV{TSYS_DIR}
        $ENV{UI_RES_DIR}
        $ENV{UI_TSYS_DIR}
        $ENV{QT_AWESOME_DIR}
        $ENV{ARGPARSE_DIR}

        ${CMAKE_SOURCE_DIR}/ui
        ${CMAKE_SOURCE_DIR}/ui/soft
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
        SoftPython
        PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
#        ${CMAKE_SOURCE_DIR}
#        ${CMAKE_SOURCE_DIR}/ui
        ${CMAKE_SOURCE_DIR}/ui/soft
)

target_link_libraries(
        SoftPython
        PUBLIC

#        ${Boost_LIBRARIES}
#        ${Python_LINK_LIBRARIES}

        SoftApi

#        Qt6::Widgets
#        Qt6::Core
#        Qt6::Gui
#        Qt6::Svg
#        Qt6::SvgWidgets
)


# ------------------------------------------
# Build the software and link the library
# to it to provide access to it.

add_executable(
        Gex_Software
        main.cpp
#        include/MainWindow.h
#        src/MainWindow.cpp
#        include/PythonInterpreter.h
#        src/PythonInterpreter.cpp
)


add_compile_definitions(
        Gex_Software
        PRIVATE
        GEX_UI_API_IMPORT
)



target_include_directories(
        Gex_Software
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
        ${Boost_INCLUDE_DIRS}
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
        SoftPython
        ${Boost_LIBRARIES}
        ${SoftApi_LIBRARY}
)

#add_custom_command(
#        TARGET Gex_Software POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${CMAKE_BINARY_DIR}/ui/SoftPython.dll
#        ${CMAKE_BINARY_DIR}/ui/SoftPython.pyd)


set(Qt_Deploy_Executable "windeployqt.exe")

set(Qt_DeployFlag --release)
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(Qt_DeployFlag --debug)
endif()


#add_custom_command(
#        TARGET
#        Gex_Software
#
#        POST_BUILD
#        COMMAND
#        ${Qt_Deploy_Executable} ${Qt_DeployFlag} ${CMAKE_CURRENT_BINARY_DIR}/Gex_Software/bin/Gex_Software.exe
#)


set(PXR_LIBRARY_DIR ${PXR_ROOT}/lib)
set(USD_PLUGIN_RELOC --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/UsdPlugin/UsdPlugin
        --library-search-dir ${PXR_LIBRARY_DIR})


add_custom_target(
        Deploy
        ALL
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name Gex
        --executable ${CMAKE_BINARY_DIR}/ui/Gex_Software.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

#        --library-search-dir ${Boost_LIBRARY_DIRS}
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
        --library ${CMAKE_BINARY_DIR}/Gex/Gex.dll
        --library ${QT_AWESOME_LIBRARY_DIR}/QtAwesome.dll
        --library ${CMAKE_BINARY_DIR}/ui/SoftApi.dll
        --library ${Boost_LIBRARIES}

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
        ${USD_PLUGIN_RELOC}
        #        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/InputRel.dll
        #        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/GraphPlugin/GraphPlugin.dll

        COMMAND
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/build_package.py
        --name SoftApi
        --directory ${CMAKE_CURRENT_BINARY_DIR}/Gex_Software/python
        --template-init ${CMAKE_CURRENT_SOURCE_DIR}/resources/__init__.py
        --search_directories ${CMAKE_CURRENT_BINARY_DIR} ${Boost_LIBRARY_DIRS}
        ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${TSYS_LIBRARY_DIR}
        ${CMAKE_BINARY_DIR}/Gex

        --library ${CMAKE_BINARY_DIR}/ui/SoftPython.dll
#        --dependencies Gex.dll

        COMMAND
        ${Qt_Deploy_Executable} ${Qt_DeployFlag} ${CMAKE_CURRENT_BINARY_DIR}/Gex_Software/bin/Gex_Software.exe
        DEPENDS
        Gex_Software
)