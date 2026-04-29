cmake_minimum_required(VERSION 3.25)
project(UiRes)

set(CMAKE_CXX_STANDARD 17)

add_definitions("/DQT_NO_KEYWORDS")
add_definitions("/DUIRES_API_EXPORT")


# Find Qt package -------------------------------------------------------------
find_package(Qt6 COMPONENTS Gui Widgets REQUIRED)
qt_standard_project_setup()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# -----------------------------------------------------------------------------

set(QT_AWESOME_LIBRARY_DIR "$ENV{QT_AWESOME_DIR}/${CMAKE_BUILD_TYPE}")

set(CMAKE_DEBUG_POSTFIX -dbg)

add_library(
        UiRes
        SHARED

        include/UiRes/uires.h
        include/UiRes/api.h

        src/uires.cpp
)


target_include_directories(
        UiRes
        INTERFACE
        $<INSTALL_INTERFACE:include>

        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $ENV{QT_AWESOME_DIR}
        ${Qt6Gui_INCLUDE_DIRS}
)


target_link_directories(
        UiRes
        PUBLIC
        ${QT_AWESOME_LIBRARY_DIR}
)


target_link_libraries(
        UiRes
        PUBLIC
        QtAwesome
        Qt6::Gui
        Qt6::Widgets
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/releases/UiRes)


install(TARGETS UiRes
        EXPORT UiRes
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
        INCLUDES DESTINATION include
        )


install(
        EXPORT UiRes
        FILE UiResTargets.cmake
        NAMESPACE UiRes::
        DESTINATION cmake
)


# Generate and install configuration file.
include(CMakePackageConfigHelpers)


configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/UiResConfig.cmake.in
        UiResConfig.cmake
        INSTALL_DESTINATION cmake
)


install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/UiResConfig.cmake
        DESTINATION cmake
)


install(
        DIRECTORY
        include/UiRes
        DESTINATION
        include
)
