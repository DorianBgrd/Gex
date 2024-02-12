cmake_minimum_required(VERSION 3.21)
project(Plugins CXX)

add_definitions(
        /DPlugins_EXPORTS
)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_VS_PLATFORM_NAME "x64")


# remove default flags provided with CMake for MSVC
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_CXX_FLAGS "/EHsc")

########################################################################################################################
# External dependencies.
########################################################################################################################


add_definitions(
        /DPlugins_EXPORT
        /DPipelineLibrary2_IMPORTS
)


add_subdirectory(ArraysPlugin)
add_subdirectory(DebugPlugin)
#add_subdirectory(FilesPlugin)
#add_subdirectory(GraphPlugin)
#add_subdirectory(IterPlugin)
#add_subdirectory(MathPlugin)
#add_subdirectory(MayaPlugin)
#add_subdirectory(OpenCommands)
#add_subdirectory(PathsPlugin)
#add_subdirectory(StringPlugin)
#add_subdirectory(UsdPlugin)