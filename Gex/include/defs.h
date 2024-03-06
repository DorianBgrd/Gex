#ifndef GEX_DEFS_H
#define GEX_DEFS_H

#include <vector>

namespace Gex
{
    class Attribute;
    class Node;
    class ScheduledNode;

    typedef std::vector<Attribute*> AttributeList;
    typedef std::vector<Node*> NodeList;
    typedef std::vector<ScheduledNode*> ScheduleNodeList;
}

#endif //GEX_DEFS_H
