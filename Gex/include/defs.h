#ifndef GEX_DEFS_H
#define GEX_DEFS_H

#include <map>
#include <vector>
#include <functional>

namespace Gex
{
    class Attribute;
    class Node;
    class ScheduledNode;
    class EvaluationNodeProfiler;

    typedef std::vector<Attribute*> AttributeList;
    typedef std::vector<Node*> NodeList;
    typedef std::vector<ScheduledNode*> ScheduleNodeList;

    typedef EvaluationNodeProfiler NodeProfiler;

    typedef std::function<void()> InvalidateCallback;
    typedef std::function<void()> ScheduleCallback;
    typedef std::map<unsigned int, InvalidateCallback> InvalidateCallbacks;
    typedef std::map<unsigned int, InvalidateCallback> ScheduleCallbacks;
    typedef unsigned int CallbackId;
}

#endif //GEX_DEFS_H
