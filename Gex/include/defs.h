#ifndef GEX_DEFS_H
#define GEX_DEFS_H

#include <map>
#include <vector>
#include <functional>

#include "ptrs.h"
#include "cntrs.h"

namespace Gex
{
    class Attribute;
    typedef std::shared_ptr<Attribute> AttributePtr;
    typedef BaseWkPtr<Attribute> AttributeWkPtr;

    enum class AttributeChange;

    class Node;
    typedef std::shared_ptr<Node> NodePtr;
    typedef BaseWkPtr<Node> NodeWkPtr;

    class CompoundNode;
    typedef std::shared_ptr<CompoundNode> CompoundNodePtr;
    typedef BaseWkPtr<CompoundNode> CompoundNodeWkPtr;

    class ScheduledNode;
    class EvaluationNodeProfiler;
    class GraphContext;

    typedef std::vector<AttributePtr> AttributeList;
    typedef std::vector<AttributeWkPtr> AttributeWkList;
    typedef std::vector<NodePtr> NodeList;
    typedef std::vector<NodeWkPtr> NodeWkList;
    typedef std::shared_ptr<ScheduledNode> ScheduledNodePtr;
    typedef std::shared_ptr<ScheduledNode> ScheduledNodeWkPtr;
    typedef std::vector<std::shared_ptr<ScheduledNode>> ScheduleNodePtrList;
    typedef std::vector<ScheduledNodeWkPtr> ScheduleNodeWkPtrList;

    class EvaluatorThread;
    typedef std::shared_ptr<EvaluatorThread> EvaluatorThreadPtr;
    typedef std::vector<EvaluatorThreadPtr> EvaluatorThreadPtrList;


    typedef EvaluationNodeProfiler NodeProfiler;

    typedef std::function<void()> InvalidateCallback;
    typedef std::function<void()> ScheduleCallback;
    typedef std::function<void()> AboutToBeDeletedCallback;
    typedef std::function<void(const AttributePtr& attribute, const AttributeChange& change)> AttributeChangeCallback;
    typedef std::function<void(std::string)> PluginLoadedCallback;
    typedef std::map<unsigned int, InvalidateCallback> InvalidateCallbacks;
    typedef std::map<unsigned int, ScheduleCallback> ScheduleCallbacks;
    typedef std::map<unsigned int, AttributeChangeCallback> AttributeChangedCallbacks;
    typedef std::map<unsigned int, AboutToBeDeletedCallback> AboutToBeDeletedCallbacks;
    typedef std::function<void(const NodePtr&)> NodeCallback;
    typedef std::function<void(const NodePtr&, bool)> NodeResCallback;
    typedef std::function<void(const GraphContext& context)> GraphCtxCallback;
    typedef unsigned int CallbackId;
}

#endif //GEX_DEFS_H
