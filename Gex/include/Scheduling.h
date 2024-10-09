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

        bool operator==(const ScheduledNode& other) const;

        bool operator==(const Gex::Node* other) const;

        bool operator==(const Gex::Node& other) const;

        bool operator==(const ScheduledNode* other) const;
    };


    GEX_API
    ScheduleNodeList ScheduleNodes(NodeList nodes, bool expand = true);

    GEX_API
    ScheduleNodeList::iterator FindScheduledNode(ScheduleNodeList list,
                                                 const Gex::Node* node);

    GEX_API
    ScheduleNodeList SubScheduledNodes(ScheduleNodeList list,
                                       Gex::Node* node);
}

#endif //GEX_SCHEDULING_H
