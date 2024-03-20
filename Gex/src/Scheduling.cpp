#include "Gex/include/Scheduling.h"
#include "Gex/include/Node.h"

#include <map>
#include <set>



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


typedef std::unordered_map<Gex::Node*, Gex::ScheduledNode*> ScheduledNodesDict;


Gex::ScheduledNode* _GetScheduledNode(Gex::Node* node,
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


std::vector<Gex::ScheduledNode*> _GetScheduledNodes(std::vector<Gex::Node*> nodes,
                                                    ScheduledNodesDict& scheduledNodes,
                                                    std::vector<Gex::Node*> graphNodes)
{
    std::vector<Gex::ScheduledNode*> results;
    std::vector<Gex::Node*> acceptedNodes;

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


Gex::ScheduleNodeList Gex::ScheduleNodes(NodeList nodes,
                                         bool expandCompounds)
{
    std::map<Gex::Node*, ScheduledNode*> schelNodes;

    std::vector<Gex::ScheduledNode*> roots;
    for (auto* node : nodes)
    {
        schelNodes[node] = node->ToScheduledNode();
    }

    for (auto sn: schelNodes)
    {
        auto sources = sn.first->UpstreamNodes();

        if (sources.empty())
        {
            roots.push_back(sn.second);
        }

        for (auto src : sources)
        {
            auto indx = schelNodes.find(src);
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
        for (auto p : iter->second)
        {
            auto* n = p;
            if (expandCompounds && n->node->IsCompound())
            {
                auto cmpSchels = CompoundNode::FromNode(n->node)->ToScheduledNodes();

//            scheduledNodes.insert(iter, cmpSchels.begin(), cmpSchels.end());
                scheduledNodes.insert(scheduledNodes.end(), cmpSchels.begin(), cmpSchels.end());

                cmpSchels.front()->previousNodes = n->previousNodes;
                for (auto src : n->previousNodes)
                {
                    auto srcindex = std::remove(src->previousNodes.begin(), src->previousNodes.end(), n);
                    if (srcindex == n->previousNodes.end())
                        continue;

                    src->previousNodes.push_back(cmpSchels.front());
                }

                cmpSchels.back()->futureNodes = n->futureNodes;
                for (auto src : n->futureNodes)
                {
                    auto dstindex = std::remove(src->futureNodes.begin(), src->futureNodes.end(), n);
                    if (dstindex == src->futureNodes.end())
                        continue;

                    src->futureNodes.push_back(cmpSchels.back());
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
