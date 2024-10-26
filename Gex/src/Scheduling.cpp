#include "Gex/include/Scheduling.h"
#include "Gex/include/Node.h"

#include <map>
#include <set>


Gex::ScheduledNode::ScheduledNode(const NodeWkPtr& node_)
{
    node = node_;
}


bool Gex::ScheduledNode::ShouldBeEvaluated() const
{
    for (ScheduledNode* n : previousNodes)
    {
        if (!n->Evaluated())
        {
            return false;
        }
    }

    return true;
}

bool Gex::ScheduledNode::Evaluate(Gex::GraphContext &context,
                                  Gex::NodeProfiler& profiler)
{
    if (node.expired())
        return false;

    return node.lock()->Compute(context, profiler);
}

bool Gex::ScheduledNode::Compute(Gex::GraphContext &context,
                                 Gex::NodeProfiler& profiler)
{
    bool result = Evaluate(context, profiler);
    evaluated = true;
    return result;
}

bool Gex::ScheduledNode::Evaluated() const
{
    return evaluated;
}


bool Gex::ScheduledNode::operator==(const ScheduledNode& other) const
{
    if (node.expired() || other.node.expired())
    {
        return false;
    }

    return (other.node.lock() == node.lock());
}


bool Gex::ScheduledNode::operator==(const Gex::NodePtr& other) const
{
    if (node.expired())
        return false;

    return (node.lock() == other);
}


bool Gex::ScheduledNode::operator==(const Gex::NodeWkPtr& other) const
{
    if (node.expired() || other.expired())
        return false;

    return (node.lock() == other.lock());
}


bool Gex::ScheduledNode::operator==(const ScheduledNode* other) const
{
    if (node.expired() || other->node.expired())
        return false;

    return (node.lock() == other->node.lock());
}



std::vector<Gex::ScheduledNode*> _SortScheduledNodes(std::vector<Gex::ScheduledNode*> toSort)
{
    std::vector<Gex::ScheduledNode*> result;
    for (Gex::ScheduledNode* node : toSort)
    {
        auto index = result.begin();
        for (Gex::ScheduledNode* previous : node->previousNodes)
        {
            auto pindex = std::find(result.begin(), result.end(), previous);
            if (pindex > index)
            {
                index = pindex;
            }
        }

        result.insert(index, node);
    }

    return result;
}


typedef std::unordered_map<Gex::NodePtr, Gex::ScheduledNode*> ScheduledNodesDict;


Gex::ScheduledNode* _GetScheduledNode(Gex::NodePtr node,
                                      ScheduledNodesDict& scheduledNodes)
{
    Gex::ScheduledNode* scheduledNode;
    try
    {
        scheduledNode = scheduledNodes.at(node);
    }
    catch (std::out_of_range&)
    {
        scheduledNode = new Gex::ScheduledNode(node);
        scheduledNodes[node] = scheduledNode;
    }

    return scheduledNode;
}


std::vector<Gex::ScheduledNode*> _GetScheduledNodes(std::vector<Gex::NodePtr> nodes,
                                                    ScheduledNodesDict& scheduledNodes,
                                                    std::vector<Gex::NodePtr> graphNodes)
{
    std::vector<Gex::ScheduledNode*> results;
    std::vector<Gex::NodePtr> acceptedNodes;

    std::set nodesSet(nodes.begin(), nodes.end());
    std::set graphSet(graphNodes.begin(), graphNodes.end());

    std::set_intersection(nodesSet.begin(),nodesSet.end(),
                          graphSet.begin(),graphSet.end(),
                          std::back_inserter(acceptedNodes));

    for (auto node : acceptedNodes)
    {
        results.push_back(_GetScheduledNode(node, scheduledNodes));
    }

    return results;
}

/*
std::vector<Gex::ScheduledNode*> Gex::_ScheduleNodes(std::vector<Gex::Node*> nodes)
{
    ScheduledNodesDict scheduledNodes;

    std::vector<Gex::ScheduledNode*> sortedScheduledNodes;
    for (auto* node : nodes)
    {
        Gex::ScheduledNode* scheduledNode = _GetScheduledNode(node, scheduledNodes);
        scheduledNode->previousNodes = _GetScheduledNodes(node->UpstreamNodes(),
                                                          scheduledNodes,
                                                          nodes);

        auto index = sortedScheduledNodes.begin();
        for (auto* previous : scheduledNode->previousNodes)
        {
            auto pindex = std::find(sortedScheduledNodes.begin(), sortedScheduledNodes.end(), previous);
            if (pindex >= index)
            {
                if (pindex != sortedScheduledNodes.end())
                    index = ++pindex;
                else
                    index = pindex;
            }
        }

        if (index == sortedScheduledNodes.end())
        {
            sortedScheduledNodes.push_back(scheduledNode);
        }
        else
        {
            sortedScheduledNodes.insert(index, scheduledNode);
        }
    }

    return sortedScheduledNodes;
}
*/

typedef Gex::ScheduledNode SchelNode;
typedef std::vector<Gex::ScheduledNode*> SchelNodeList;
typedef std::map<SchelNode*, unsigned int> SchelNodeDict;
typedef std::map<unsigned int, SchelNodeList> SchelNodeSortedDict;


void ResolveNodesLevels(SchelNode* node, SchelNodeDict& nodes, unsigned int level)
{
    if (nodes.find(node) != nodes.end())
    {
        auto l = nodes.at(node);
        if (level > l)
        {
            nodes[node] = level;
        }
    }
    else
    {
        nodes[node] = level;
    }

    for (auto nextNode : node->futureNodes)
    {
        ResolveNodesLevels(nextNode, nodes, level + 1);
    }
}


Gex::ScheduleNodeList Gex::ScheduleNodes(NodeList nodes, bool expand)
{
    std::map<Gex::NodePtr, ScheduledNode*> schelNodes;

    std::vector<Gex::ScheduledNode*> roots;
    for (const auto& node : nodes)
    {
        schelNodes[node] = node->ToScheduledNode();
    }

    for (const auto& sn: schelNodes)
    {
        auto sources = sn.first->UpstreamNodes();

        if (sources.empty())
        {
            roots.push_back(sn.second);
        }

        for (const auto& src : sources)
        {
            if (!src)
                continue;

            auto indx = schelNodes.find(src.ToShared());
            if (indx == schelNodes.end())
            {
                continue;
            }

            auto* scheclSrc = indx->second;
            sn.second->previousNodes.push_back(scheclSrc);
            scheclSrc->futureNodes.push_back(sn.second);
        }
    }

    SchelNodeDict nodesPerLevels;
    for (auto* snode : roots)
    {
        ResolveNodesLevels(snode, nodesPerLevels, 0);
    }

    SchelNodeSortedDict dict;
    for (auto p : nodesPerLevels)
    {
        if (dict.find(p.second) == dict.end())
        {
            dict[p.second] = SchelNodeList();
        }

        dict.at(p.second).push_back(p.first);
    }


    std::vector<Gex::ScheduledNode*> scheduledNodes;
    for (auto iter = dict.begin(); iter != dict.end(); iter++)
    {
        // For each node in scheduled nodes.
        for (auto p : iter->second)
        {
            auto* n = p;

            if (!n->node.expired())
                continue;

            NodePtr lockedNode = n->node.lock();
            // If it is a compound, expand it.
            if (expand && lockedNode->IsCompound())
            {
                // Transform the compound to a series of scheduled nodes.
                auto cmpSchels = lockedNode->ToScheduledNodes();

                // Append resulting scheduled nodes from resulting scheduled.
                scheduledNodes.insert(scheduledNodes.end(), cmpSchels.begin(), cmpSchels.end());

                // Previous nodes of the first compound series node is the current
                // node previous nodes.
                cmpSchels.front()->previousNodes = n->previousNodes;

                // Then, for each previous nodes of current node, remove it from
                // their own future nodes and replace it with the first nodes from
                // compound series.
                for (auto src : n->previousNodes)
                {
                    auto srcindex = std::remove(src->futureNodes.begin(), src->futureNodes.end(), n);
                    if (srcindex == src->futureNodes.end())
                        continue;

                    src->futureNodes.push_back(cmpSchels.front());
                }

                // Then, for each future nodes of current node, remove it from
                // their own previous nodes and replace it with the last nodes
                // from compound series.
                cmpSchels.back()->futureNodes = n->futureNodes;
                for (auto src : n->futureNodes)
                {
                    auto dstindex = std::remove(src->previousNodes.begin(), src->previousNodes.end(), n);
                    if (dstindex == src->previousNodes.end())
                        continue;

                    src->previousNodes.push_back(cmpSchels.back());
                }

                delete n;
            }
            else
            {
                scheduledNodes.push_back(n);
            }
        }

    }

    return scheduledNodes;
}


Gex::ScheduleNodeList::iterator Gex::FindScheduledNode(
        Gex::ScheduleNodeList scheduled,
        const Gex::NodePtr node)
{
    auto pred = [node](ScheduledNode* n){return (*n == node);};

    return std::find_if(scheduled.begin(), scheduled.end(), pred);
}


Gex::ScheduleNodeList Gex::SubScheduledNodes(ScheduleNodeList list, Gex::NodePtr node)
{
    ScheduleNodeList subGraph;

    auto pred = [node](ScheduledNode* s)
    {
        return (!s->node.expired() && s->node.lock() == node);
    };

    auto iter = std::find_if(list.begin(), list.end(), pred);
    if (iter == list.end())
        return subGraph;

    std::set<ScheduledNode*> search = {*iter};

    subGraph.push_back(*iter);

    for (;iter != list.end(); iter++)
    {
        auto currentNode = (*iter);
        auto previousNodes = currentNode->previousNodes;
        for (auto previousNode : previousNodes)
        {
            if (search.find(previousNode) != search.end())
            {
                subGraph.push_back(currentNode);
                search.insert(currentNode);
                break;
            }
        }
    }

    return subGraph;
}

