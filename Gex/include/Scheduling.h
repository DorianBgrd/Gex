#ifndef GEX_SCHEDULING_H
#define GEX_SCHEDULING_H

#include "api.h"
#include "defs.h"

namespace Gex
{
    class Node;
    class GraphContext;

    struct GEX_API ScheduledNode
    {
        Node *node;
        ScheduleNodeList previousNodes;
        ScheduleNodeList futureNodes;
        bool evaluated = false;
        bool success = false;

        ScheduledNode(Node *node);

        bool ShouldBeEvaluated() const;

        virtual bool Evaluate(GraphContext &context,
                              NodeProfiler &profiler);

        bool Compute(GraphContext &context,
                     NodeProfiler &profiler);

        bool Evaluated() const;
    };


    GEX_API
    ScheduleNodeList ScheduleNodes(NodeList nodes, bool expandCompounds = true);
}

#endif //GEX_SCHEDULING_H
