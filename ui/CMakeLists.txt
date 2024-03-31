cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Gex_Ui CXX)

set(PROJECT_NAME Gex_Ui)


add_definitions("/DQT_NO_KEYWORDS")


# Find Qt package -------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets Core Gui Svg SvgWidgets REQUIRED)
qt_standard_project_setup()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt Awesome directory --------------------------------------------------------
set(QT_AWESOME_LIBRARY_DIR "$ENV{QT_AWESOME_DIR}/${CMAKE_BUILD_TYPE}")
# -------------------------------------------------------------------------


add_library(
        Gex_ui
        SHARED
        include/AttributeEditor.h
        src/AttributeEditor.cpp
        include/BaseGraph.h
        src/BaseGraph.cpp
        include/commands.h
        src/commands.cpp
        include/NodeGraph.h
        src/NodeGraph.cpp
        include/UiStatus.h
        src/UiStatus.cpp
        include/ProfileViewer.h
        src/ProfileViewer.cpp
        include/GraphView.h
        src/GraphView.cpp
        include/PropertyEditor.h
        src/PropertyEditor.cpp)

add_compile_definitions(Gex_ui PRIVATE GEX_UI_API_EXPORT)

#qt6_generate_moc(
#        include/AttributeEditor.h
#        include/BaseGraph.h
#        include/commands.h
#        include/NodeGraph.h
#        TARGET
#        Gex_ui
#)


target_include_directories(
        Gex_ui
        PUBLIC
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
)


target_link_directories(
        Gex_ui
        PUBLIC
        $ENV{RAPIDJSON_DIR}
        ${TSYS_LIBRARY_DIR}
        $ENV{UI_TSYS_DIR}/UiTSys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        ${QT_AWESOME_LIBRARY_DIR}
        ${CMAKE_BINARY_DIR}
)


target_link_libraries(
        Gex_ui
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
        QtAwesome
)


add_subdirectory(
        soft
)


add_dependencies(
        Gex_Software
        Gex_ui
)
