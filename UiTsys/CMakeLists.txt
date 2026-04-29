cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(TypeEngine CXX)

set(PROJECT_NAME TypeEngine)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_definitions("/DUI_TSYS_EXPORT")


# Find Qt package -------------------------------------------------------------
add_definitions("/DQT_NO_KEYWORDS")
find_package(Qt6 COMPONENTS Widgets Core Gui Svg REQUIRED)
qt_standard_project_setup()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# -----------------------------------------------------------------------------

set(CMAKE_DEBUG_POSTFIX -dbg)


add_library(
        UiTsys
        SHARED
        include/UiTsys/api.h
        include/UiTsys/uitsys.h
        include/UiTsys/defaultTypes.h
        src/uitsys.cpp
        src/defaultTypes.cpp
)


#qt6_wrap_cpp(include/uitsys.h include/defaultTypes.h src/uitsys.cpp
#        TARGET UiTSys)


target_include_directories(
        UiTsys

        INTERFACE
        $<INSTALL_INTERFACE:include>

        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Tsys_INCLUDE_DIRS}
        ${UiRes_INCLUDE_DIRS}
        $ENV{RAPIDJSON_DIR}
        $ENV{UI_RES_DIR}
        $ENV{QT_AWESOME_DIR}
        ${pybind11_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)


string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)


target_link_directories(
        UiTsys
        PUBLIC
        $ENV{TSYS_DIR}/Tsys/cmake-build-${BUILD_TYPE}
        $ENV{UI_RES_DIR}/UiRes/cmake-build-${BUILD_TYPE}
        $ENV{QT_AWESOME_DIR}/${CMAKE_BUILD_TYPE}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        UiTsys
        PUBLIC
        ${Python_LINK_LIBRARIES}
        ${Boost_LIBRARIES}
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Tsys
        UiRes
        QtAwesome
)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/releases/UiTsys-Python${Python_USE_VERSION})


install(TARGETS UiTsys
        EXPORT UiTsys
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
        INCLUDES DESTINATION include
        )


install(
        EXPORT UiTsys
        FILE UiTsysTargets.cmake
        NAMESPACE UiTsys::
        DESTINATION cmake
)


# Generate and install configuration file.
include(CMakePackageConfigHelpers)


configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/UiTsysConfig.cmake.in
        UiTsysConfig.cmake
        INSTALL_DESTINATION cmake
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/UiTsysConfig.cmake
        DESTINATION cmake
)

install(
        DIRECTORY
        include/UiTsys
        DESTINATION
        include
)
