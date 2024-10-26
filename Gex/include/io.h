#ifndef GEX_IO_H
#define GEX_IO_H

#include "api.h"
#include "defs.h"
#include "Node.h"

namespace Gex
{
    GEX_API void ExportToFile(const std::string& filepath, const std::string& str,
                              Feedback* result=nullptr);

    GEX_API Feedback SaveGraph(NodePtr graph, const std::string& filepath);

    GEX_API NodePtr LoadGraph(const std::string& filepath, Feedback* success=nullptr);

    GEX_API NodePtr LoadGraphString(const std::string& content, Feedback* success=nullptr);

    GEX_API Feedback ExportAsCompound(NodeList nodes, NodePtr graph, const std::string& directory,
                                      const std::string& name, bool force=false);

    GEX_API Feedback ExportToString(NodePtr graph, std::string& str,
                                    bool pretty=true);

//    GEX_API Feedback ExportNodes(Node* graph, NodeList nodes,
//                                 const std::string& filepath);

    GEX_API NodeList ImportNodes(const std::string& filepath,
                                 CompoundNode* destination,
                                 Feedback* success=nullptr);

    GEX_API bool ReloadNode(Gex::NodePtr node);

//    GEX_API std::vector<std::string> ListAvailableReferences();
}

#endif //GEX_IO_H
