#ifndef GEX_DEFS_H
#define GEX_DEFS_H

#include <map>
#include <vector>
#include <functional>

namespace Gex
{
    class Attribute;
    enum class AttributeChange;
    class Node;
    class ScheduledNode;
    class EvaluationNodeProfiler;
    class GraphContext;

    typedef std::vector<Attribute*> AttributeList;
    typedef std::vector<Node*> NodeList;
    typedef std::vector<ScheduledNode*> ScheduleNodeList;

    typedef EvaluationNodeProfiler NodeProfiler;

    typedef std::function<void()> InvalidateCallback;
    typedef std::function<void()> ScheduleCallback;
    typedef std::function<void(Attribute* attribute, const AttributeChange& change)> AttributeChangeCallback;
    typedef std::function<void(std::string)> PluginLoadedCallback;
    typedef std::map<unsigned int, InvalidateCallback> InvalidateCallbacks;
    typedef std::map<unsigned int, InvalidateCallback> ScheduleCallbacks;
    typedef std::map<unsigned int, AttributeChangeCallback> AttributeChangedCallbacks;
    typedef std::function<void(Node*)> NodeCallback;
    typedef std::function<void(Node*, bool)> NodeResCallback;
    typedef std::function<void(const GraphContext& context)> GraphCtxCallback;
    typedef unsigned int CallbackId;
}

#endif //GEX_DEFS_H
