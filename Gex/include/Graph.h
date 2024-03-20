#ifndef NODELIBRARY2COMPLETE_GRAPH_H
#define NODELIBRARY2COMPLETE_GRAPH_H

#include "api.h"
#include "defs.h"
#include "Node.h"
#include <vector>
#include <set>
#include <functional>
#include "rapidjson/document.h"

#include "Status.h"
#include "Profiler.h"
#include "Tsys/include/tsys.h"
#include "SafePtrs/safeptr.h"


namespace Gex
{

    class Graph;
    class Attribute;
    class NodeEvaluator;


    class GEX_API GraphContext
    {
        friend NodeEvaluator;
        std::vector<std::string> resources;

    public:
        GraphContext();

        GraphContext(const GraphContext& other);

        void RegisterResource(std::string resource);

        std::vector<std::string> Resources() const;

        PropertyWrapper Property(std::string name) const;
    };


    class GEX_API Graph: public CompoundNode
    {
        friend GraphContext;

        AttributeList inputs;
        ScheduleNodeList scheduledNodes;
        bool valid = false;

        CallbackId invalidCbId = 0;
        CallbackId scheduleCbId = 0;
        InvalidateCallbacks invalidateCallbacks;
        ScheduleCallbacks scheduleCallbacks;

        PropertyMap properties;

    public:
        /**
         * Evaluation modifiers.
         */

        bool IsValid() const;

        void Invalidate();

        void Schedule();

        CallbackId AddInvalidateCallback(InvalidateCallback callback);

        void RemoveInvalidateCallback(CallbackId callback);

        void ClearInvalidateCallbacks();

        CallbackId AddScheduleCallback(ScheduleCallback callback);

        void RemoveScheduleCallback(CallbackId callback);

        void ClearScheduleCallbacks();

    public:
        NodeList DuplicateNodes(NodeList sources, bool copyLinks,
                                Node* parent, Graph* graph=nullptr);

    public:
        NodeList DuplicateNodes(NodeList sources, bool copyLinks=false);

        Gex::Node* ToCompound(NodeList sources, bool duplicate=false,
                              bool keepExternalConnections=true);

        Attribute* FindAttribute(std::string attr) const;

        bool Run(Profiler& profile, unsigned int threads=1,
                 NodeCallback nodeStarted=nullptr,
                 NodeResCallback nodeDone=nullptr,
                 GraphCtxCallback evalDone=nullptr);

        void FinalizeEvaluation(const GraphContext& context) const;

        void AttributeChanged(Attribute* attr, AttributeChange change) override;
    };
}



#endif //NODELIBRARY2COMPLETE_GRAPH_H
