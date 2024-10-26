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
        NodeWkPtr node;
        ScheduleNodeList previousNodes;
        ScheduleNodeList futureNodes;
        bool evaluated = false;
        bool success = false;

        explicit ScheduledNode(const NodeWkPtr& node);

        bool ShouldBeEvaluated() const;

        virtual bool Evaluate(GraphContext &context,
                              NodeProfiler &profiler);

        bool Compute(GraphContext &context,
                     NodeProfiler &profiler);

        bool Evaluated() const;

        bool operator==(const ScheduledNode& other) const;

        bool operator==(const Gex::NodePtr& other) const;

        bool operator==(const Gex::NodeWkPtr& other) const;

        bool operator==(const ScheduledNode* other) const;
    };


    GEX_API
    ScheduleNodeList ScheduleNodes(NodeList nodes, bool expand = true);

    GEX_API
    ScheduleNodeList::iterator FindScheduledNode(ScheduleNodeList list,
                                                 const Gex::NodePtr node);

    GEX_API
    ScheduleNodeList SubScheduledNodes(ScheduleNodeList list,
                                       Gex::NodePtr node);
}

#endif //GEX_SCHEDULING_H
