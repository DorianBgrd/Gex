cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Gex_Ui CXX)

set(PROJECT_NAME Gex_Ui)


add_definitions("/DQT_NO_KEYWORDS")


# Find Qt package -------------------------------------------------------------
qt_standard_project_setup()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt Awesome directory --------------------------------------------------------
set(QT_AWESOME_LIBRARY_DIR "$ENV{QT_AWESOME_DIR}/${CMAKE_BUILD_TYPE}")
# -------------------------------------------------------------------------

set(
        GEX_UI_INCLUDES
        include/Gex_ui/AttributeEditor.h
        include/Gex_ui/BaseGraph.h
        include/Gex_ui/Commands.h
        include/Gex_ui/NodeGraph.h
        include/Gex_ui/UiStatus.h
        include/Gex_ui/ProfileViewer.h
        include/Gex_ui/GraphView.h
        include/Gex_ui/PluginLoader.h
        include/Gex_ui/NodeViewer.h
        include/Gex_ui/Timeline.h
        include/Gex_ui/ViewContexts.h
        include/Gex_ui/NodeUiRegistry.h
        include/Gex_ui/ScheduleGraph.h
)

add_library(
        Gex_ui
        SHARED
        ${GEX_UI_INCLUDES}
        src/AttributeEditor.cpp
        src/BaseGraph.cpp
        src/Commands.cpp
        src/NodeGraph.cpp
        src/UiStatus.cpp
        src/ProfileViewer.cpp
        src/GraphView.cpp
        src/PluginLoader.cpp
        src/NodeViewer.cpp
        src/Timeline.cpp
        src/ViewContexts.cpp
        src/NodeUiRegistry.cpp
        src/ScheduleGraph.cpp
)

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
        ${CMAKE_SOURCE_DIR}/Gex
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6Svg_INCLUDE_DIRS}
        ${Qt6SvgWidgets_INCLUDE_DIRS}
        ${Tsys_INCLUDE_DIRS}
        $ENV{UI_RES_DIR}
        $ENV{UI_TSYS_DIR}
        $ENV{QT_AWESOME_DIR}
)


target_link_directories(
        Gex_ui
        PUBLIC
        $ENV{RAPIDJSON_DIR}
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
        Tsys
        UiTsys
        UiRes
        Gex
        QtAwesome
)




install(TARGETS Gex_ui DESTINATION bin
        CONFIGURATIONS Debug Release)

install(FILES ${GEX_UI_INCLUDES}
        DESTINATION include/Gex_ui)