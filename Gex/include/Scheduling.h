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
        ScheduleNodeWkPtrList previousNodes;
        ScheduleNodeWkPtrList futureNodes;
        unsigned int sourceEvaluated = 0;

        bool evaluated = false;
        bool success = false;

        explicit ScheduledNode(const NodeWkPtr& node);

        explicit ScheduledNode(const ScheduledNode& other);

        explicit ScheduledNode(const ScheduledNodePtr& other);

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

        operator bool() const;

        void SignalEvaluated() const;

    protected:
        void ReceiveEvaluated();
    };

    GEX_API
    ScheduleNodePtrList ScheduleNodes(const NodeList& nodes, bool expand = true);

    GEX_API
    ScheduleNodePtrList::iterator FindScheduledNode(ScheduleNodePtrList list,
                                                    const Gex::NodePtr& node);

    GEX_API
    ScheduleNodePtrList CopyScheduledNodes(const ScheduleNodePtrList& nodes);

    GEX_API
    ScheduleNodePtrList SubScheduledNodes(const ScheduleNodePtrList& list,
                                          const Gex::NodePtr& node);
}

#endif //GEX_SCHEDULING_H
