#ifndef GEX_IO_H
#define GEX_IO_H

#include "api.h"
#include "Graph.h"
#include "Node.h"

namespace Gex
{
    GEX_API void ExportToFile(const std::string& filepath, rapidjson::Document& json,
                              Feedback* result=nullptr);

    GEX_API Feedback SaveGraph(Graph* graph, const std::string& filepath);

    GEX_API Graph* LoadGraph(const std::string& filepath, Feedback* success=nullptr);

    GEX_API Feedback ExportNodes(NodeList nodes, Graph* graph, const std::string& directory,
                                 const std::string& name, bool force=false);
}

#endif //GEX_IO_H
