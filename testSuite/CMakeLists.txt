cmake_minimum_required(VERSION 3.24)

project(TestSuite)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DTS_API_EXPORT)

#set(CMAKE_DEBUG_POSTFIX -dbg)

set(
        TS_SOURCES

        src/main.cpp
)

set(
        TS_INCLUDES

        include/TS/TestNodes.h
        include/TS/TestEvaluator.h
        include/Tests/TestEvaluator.h
)


add_executable(
        TS

        ${TS_SOURCES}
)

message("TSYS_INCLUDE_DIRS : ${TSYS_INCLUDE_DIRS}")


target_include_directories(
        TS

        INTERFACE
        $<INSTALL_INTERFACE:include>

        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Tsys/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Gex/include>
        $ENV{RAPIDJSON_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
        ${pybind11_INCLUDE_DIRS}

)

target_link_directories(
        TS
        PUBLIC

        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${TS_LIBRARY_DIRS}
)


target_link_libraries(
        TS
        PUBLIC

        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
        Tsys
        Gex
)


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/releases/Tsys-Python${Python_USE_VERSION})




if (${Build_UsdPlugin})
    set(PXR_LIBRARY_DIR ${PXR_ROOT}/lib)

    set(USD_PLUGIN_RELOC --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/UsdPlugin/UsdPlugin
            --library-search-dir ${PXR_LIBRARY_DIR})
endif()


add_custom_target(
        TestSuite-Deploy
        ALL
        python $ENV{BUILD_UTILS_DIR}/BuildUtils/deploy.py

        --name TestSuite
        --executable ${CMAKE_CURRENT_BINARY_DIR}/TS.exe
        --destination ${CMAKE_CURRENT_BINARY_DIR}

        #        --library-search-dir ${Boost_LIBRARY_DIRS}
        --library-search-dir ${Python_LIBRARY_DIRS}
        --library-search-dir ${Python_ROOT_DIR}
        --library-search-dir ${CMAKE_CURRENT_SOURCE_DIR}
        --library-search-dir ${CMAKE_BINARY_DIR}/Tsys
        --library-search-dir ${CMAKE_BINARY_DIR}/Gex
        --library-search-dir ${CMAKE_BINARY_DIR}

        --library ${CMAKE_BINARY_DIR}/Tsys/Tsys.dll
        --library ${CMAKE_BINARY_DIR}/Gex/Gex.dll
        --library Python${Python_VERSION_NUMBER}.dll

        --extra-directory plugins

        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/ArraysPlugin/ArraysPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/DebugPlugin/DebugPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/FilesPlugin/FilesPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/IterPlugin/IterPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/MathPlugin/MathPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/MayaPlugin/MayaPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/PathsPlugin/PathsPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/StringPlugin/StringPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/ImageManipPlugin/ImageManipPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/InputRelPlugin/InputRelPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/JsonPlugin/JsonPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/PythonPlugin/PythonPlugin
        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/Gex_plugins/GeometryPlugin/GeometryPlugin
        ${USD_PLUGIN_RELOC}
        #        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/InputRel.dll
        #        --add-extra-directory plugins ${CMAKE_BINARY_DIR}/plugins/GraphPlugin/GraphPlugin.dll

#        COMMAND
#        ${CMAKE_CURRENT_BINARY_DIR}/TestSuite/TS/bin/TS.exe
#        ${Qt_Deploy_Executable} ${Qt_DeployFlag} ${CMAKE_CURRENT_BINARY_DIR}/Gex_App/bin/Gex_App.exe
        DEPENDS
        TS
)