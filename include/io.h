#ifndef GEX_IO_H
#define GEX_IO_H

#include "api.h"
#include "Graph.h"

namespace Gex
{
    GEX_API Feedback SaveGraph(Graph* graph, const std::string& filepath);

    GEX_API Graph* LoadGraph(std::string filepath, Feedback* success=nullptr);
}

#endif //GEX_IO_H
