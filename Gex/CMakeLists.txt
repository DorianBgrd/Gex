

##########################
# Set sources for library.
##########################

set(
        GEX_INCLUDES
        include/Gex/Attribute.h
        include/Gex/Attribute_Wrap.h
        include/Gex/cntrs.h
        include/Gex/Config.h
        include/Gex/defs.h
        include/Gex/Evaluation.h
        include/Gex/Gex.h
        include/Gex/Graph_Wrap.h
        include/Gex/io.h
        include/Gex/io_Wrap.h
        include/Gex/Metadatas.h
        include/Gex/Node.h
        include/Gex/Node_Wrap.h
        include/Gex/NodeAttributeData.h
        include/Gex/NodeFactory.h
        include/Gex/NodeFactory_Wrap.h
        include/Gex/PluginLoader.h
        include/Gex/PluginLoader_Wrap.h
        include/Gex/Profiler.h
        include/Gex/Profiler_Wrap.h
        include/Gex/ptrs.h
        include/Gex/References.h
        include/Gex/References_Wrap.h
        include/Gex/References_Wrap.h
        include/Gex/Scheduling.h
        include/Gex/Status.h
        include/Gex/Status_Wrap.h
        include/Gex/UndoCommands.h
        include/Gex/UndoStack.h
        include/Gex/utils.h
        include/Gex/wraputils.h
        include/Gex/Wrappers.h
        include/Gex/Commands.h
        include/Gex/Interpreter.h
)

set(
        GEX_SOURCES

        # Cpp source files.
        src/Attribute.cpp
        src/Evaluation.cpp
        src/Node.cpp
        src/NodeAttributeData.cpp
        src/NodeFactory.cpp
        src/PluginLoader.cpp
        src/Config.cpp
        include/Gex/Status.h
        src/Status.cpp
        include/Gex/Gex.h
        include/Gex/io.h
        src/io.cpp
        include/Gex/Profiler.h
        src/Profiler.cpp
        include/Gex/utils.h
        src/utils.cpp
        include/Gex/defs.h
        include/Gex/Scheduling.h
        src/Scheduling.cpp
        include/Gex/Metadatas.h
        src/Metadatas.cpp
        src/References.cpp
        include/Gex/UndoStack.h
        src/UndoStack.cpp
        src/UndoStack_Wrap.cpp
        include/Gex/UndoCommands.h
        src/UndoCommands.cpp

        # Python wrapping file.
        include/Gex/Attribute_Wrap.h
        src/Attribute_Wrap.cpp
        include/Gex/Graph_Wrap.h
        src/Graph_Wrap.cpp
        include/Gex/io_Wrap.h
        src/io_Wrap.cpp
        include/Gex/Node_Wrap.h
        src/Node_Wrap.cpp
        include/Gex/NodeAttributeData_Wrap.h
        src/NodeAttributeData_Wrap.cpp
        include/Gex/NodeFactory_Wrap.h
        src/NodeFactory_Wrap.cpp
        include/Gex/PluginLoader_Wrap.h
        src/PluginLoader_Wrap.cpp
        include/Gex/Status_Wrap.h
        src/Status_Wrap.cpp
        src/References_Wrap.cpp
        src/Profiler_Wrap.cpp
        include/Gex/Wrappers.h
        src/Wrappers.cpp
        include/Gex/Commands.h
        src/Commands.cpp

        include/Gex/Interpreter.h
        src/Interpreter.cpp
)


set(
        GEX_INCLUDE_DIRECTORIES

        ${CMAKE_SOURCE_DIR}
        $ENV{RAPIDJSON_DIR}
        ${CMAKE_SOURCE_DIR}/Tsys/include
        $ENV{SAFEPTRS_DIR}
        ${pybind11_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)

set(
        GEX_LINK_DIRECTORIES

        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)

##############################
# Shared Library
#############################

add_library(
        Gex
        SHARED
        ${GEX_SOURCES}
)


target_include_directories(
        Gex
        PUBLIC
        ${GEX_INCLUDE_DIRECTORIES}
)


target_link_directories(
        Gex
        PUBLIC

        ${GEX_LINK_DIRECTORIES}
)

target_link_libraries(
        Gex
        PUBLIC
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
        Tsys
)


##############################
# Static Library
#############################


add_library(
        Gex_Static
        STATIC
        ${GEX_SOURCES}
)


target_include_directories(
        Gex_Static
        PUBLIC
        ${GEX_INCLUDE_DIRECTORIES}
)


target_link_directories(
        Gex_Static
        PUBLIC
        ${GEX_LINK_DIRECTORIES}
)

target_link_libraries(
        Gex_Static
        PUBLIC
        ${Boost_LIBRARIES}
        ${Python_LINK_LIBRARIES}
        ${TSYS_STATIC_LIBRARIES}
)


target_compile_definitions(Gex PRIVATE /DGEX_API_EXPORT)

install(TARGETS Gex DESTINATION bin
        CONFIGURATIONS Debug Release)

install(FILES ${GEX_INCLUDES} DESTINATION include/Gex)