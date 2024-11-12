

##########################
# Set sources for library.
##########################

set(
        GEX_SOURCES

        # Cpp source files.
        src/Attribute.cpp
        src/Evaluation.cpp
        src/Node.cpp
        src/NodeAttributeData.cpp
        src/NodeFactory.cpp
        src/PluginLoader.cpp
        include/Config.h
        src/Config.cpp
        include/Status.h
        include/Gex.h
        include/io.h
        src/io.cpp
        include/Profiler.h
        src/Profiler.cpp
        include/utils.h
        src/utils.cpp
        include/defs.h
        include/Scheduling.h
        src/Scheduling.cpp
        include/Metadatas.h
        src/Metadatas.cpp
        src/References.cpp
        include/UndoStack.h
        src/UndoStack.cpp
        src/UndoStack_Wrap.cpp
        include/UndoCommands.h
        src/UndoCommands.cpp

        # Python wrapping file.
        include/Attribute_Wrap.h
        src/Attribute_Wrap.cpp
        include/Graph_Wrap.h
        src/Graph_Wrap.cpp
        include/io_Wrap.h
        src/io_Wrap.cpp
        include/Node_Wrap.h
        src/Node_Wrap.cpp
        include/NodeFactory_Wrap.h
        src/NodeFactory_Wrap.cpp
        include/PluginLoader_Wrap.h
        src/PluginLoader_Wrap.cpp
        include/Status_Wrap.h
        src/Status_Wrap.cpp
        src/References_Wrap.cpp
)


set(
        GEX_INCLUDE_DIRECTORIES

        ${CMAKE_SOURCE_DIR}
        $ENV{RAPIDJSON_DIR}
        $ENV{TSYS_DIR}
        $ENV{SAFEPTRS_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)

set(
        GEX_LINK_DIRECTORIES

        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
        ${TSYS_LIBRARY_DIR}
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
        ${Python_LIBRARIES}
        TSys
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
        ${Python_LIBRARIES}
        TSys_Static
)


target_compile_definitions(Gex PRIVATE /DGEX_API_EXPORT)