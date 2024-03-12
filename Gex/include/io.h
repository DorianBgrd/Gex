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

    GEX_API Feedback ExportAsCompound(NodeList nodes, Graph* graph, const std::string& directory,
                                      const std::string& name, bool force=false);

    GEX_API Feedback ExportNodes(Graph* graph, NodeList nodes,
                                 const std::string& filepath);

    GEX_API NodeList ImportNodes(const std::string& filepath,
                                 Graph* destinationGraph,
                                 CompoundNode* compound=nullptr,
                                 Feedback* success=nullptr);
}

#endif //GEX_IO_H
