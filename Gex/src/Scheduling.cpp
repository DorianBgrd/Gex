#include "Gex/include/Scheduling.h"
#include "Gex/include/Node.h"

#include <map>
#include <set>


Gex::ScheduledNode::ScheduledNode(const NodeWkPtr& node_)
{
    node = node_;
}


Gex::ScheduledNode::ScheduledNode(const ScheduledNode& other)
{
    node = other.node;
    futureNodes = other.futureNodes;
    previousNodes = other.previousNodes;
}


Gex::ScheduledNode::ScheduledNode(const ScheduledNodePtr& src)
{
    node = src->node;
    futureNodes = src->futureNodes;
    previousNodes = src->previousNodes;
}


bool Gex::ScheduledNode::ShouldBeEvaluated() const
{
    return sourceEvaluated >= previousNodes.size();
}


bool Gex::ScheduledNode::Evaluate(Gex::GraphContext &context,
                                  Gex::NodeProfiler& profiler)
{
    return node->Compute(context, profiler);
}

bool Gex::ScheduledNode::Compute(Gex::GraphContext &context,
                                 Gex::NodeProfiler& profiler)
{
    bool result = Evaluate(context, profiler);
    evaluated = true;

    SignalEvaluated();

    return result;
}

bool Gex::ScheduledNode::Evaluated() const
{
    return evaluated;
}


bool Gex::ScheduledNode::operator==(const ScheduledNode& other) const
{
    if (!node || !other.node)
    {
        return false;
    }

    return (other.node.lock() == node.lock());
}


bool Gex::ScheduledNode::operator==(const Gex::NodePtr& other) const
{
    if (!node)
        return false;

    return (node.lock() == other);
}


bool Gex::ScheduledNode::operator==(const Gex::NodeWkPtr& other) const
{
    if (!node || !other)
        return false;

    return (node == other);
}


bool Gex::ScheduledNode::operator==(const ScheduledNode* other) const
{
    if (!node || !other->node)
        return false;

    return (node == other->node);
}


Gex::ScheduledNode::operator bool() const
{
    return node;
}


void Gex::ScheduledNode::SignalEvaluated() const
{
    for (const auto& future : futureNodes)
    {
        future->ReceiveEvaluated();
    }
}


void Gex::ScheduledNode::ReceiveEvaluated()
{
    sourceEvaluated += 1;
}


typedef std::unordered_map<Gex::NodePtr, Gex::ScheduledNode*> ScheduledNodesDict;


typedef Gex::ScheduledNodePtr SchelNode;
typedef std::vector<Gex::ScheduledNodePtr> SchelNodeList;
typedef std::map<SchelNode, unsigned int> SchelNodeDict;
typedef std::map<unsigned int, SchelNodeList> SchelNodeSortedDict;


void ResolveNodesLevels(SchelNode node, SchelNodeDict& nodes, unsigned int level)
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

    for (const auto& nextNode : node->futureNodes)
    {
        ResolveNodesLevels(nextNode, nodes, level + 1);
    }
}


Gex::ScheduleNodePtrList Gex::CopyScheduledNodes(
        const Gex::ScheduleNodePtrList& sources)
{
    std::unordered_map<Gex::NodePtr, Gex::ScheduledNodePtr> duplis;

    Gex::ScheduleNodePtrList copiedGraph;
    for (const auto& schelNode : sources)
    {
        if (!schelNode)
            continue;

        auto copy = std::make_shared<Gex::ScheduledNode>(
                Gex::ScheduledNode(schelNode));
        duplis[schelNode->node.lock()] = copy;

        copiedGraph.push_back(copy);
    }

    for (auto& copy : copiedGraph)
    {
        Gex::ScheduleNodeWkPtrList newFuture;
        for (const auto& future : copy->futureNodes)
        {
            auto match = duplis.find(future->node.lock());
            if (match != duplis.end())
                newFuture.emplace_back(match->second);
            else
                newFuture.push_back(future);
        }
        copy->futureNodes = newFuture;

        Gex::ScheduleNodeWkPtrList newPrevious;
        for (const auto& previous : copy->previousNodes)
        {
            auto match = duplis.find(previous->node.lock());
            if (match != duplis.end())
                newPrevious.emplace_back(match->second);
            else
                newPrevious.push_back(previous);
        }
        copy->previousNodes = newPrevious;
    }

    return copiedGraph;
}


Gex::ScheduleNodePtrList Gex::ScheduleNodes(const NodeList& nodes, bool expand)
{
    std::map<Gex::NodePtr, ScheduledNodePtr> schelNodes;

    ScheduleNodePtrList roots;
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

            auto scheclSrc = indx->second;
            sn.second->previousNodes.push_back(scheclSrc);
            scheclSrc->futureNodes.push_back(sn.second);
        }
    }

    SchelNodeDict nodesPerLevels;
    for (const auto& snode : roots)
    {
        ResolveNodesLevels(snode, nodesPerLevels, 0);
    }

    SchelNodeSortedDict dict;
    for (const auto& p : nodesPerLevels)
    {
        if (dict.find(p.second) == dict.end())
        {
            dict[p.second] = SchelNodeList();
        }

        dict.at(p.second).push_back(p.first);
    }


    std::vector<Gex::ScheduledNodePtr> scheduledNodes;
    for (auto iter = dict.begin(); iter != dict.end(); iter++)
    {
        // For each node in scheduled nodes.
        for (const auto& p : iter->second)
        {
            auto n = p;

            if (!n->node)
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
                for (const auto& src : n->previousNodes)
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
                for (const auto& src : n->futureNodes)
                {
                    auto dstindex = std::remove(src->previousNodes.begin(), src->previousNodes.end(), n);
                    if (dstindex == src->previousNodes.end())
                        continue;

                    src->previousNodes.push_back(cmpSchels.back());
                }
            }
            else
            {
                scheduledNodes.push_back(n);
            }
        }

    }

    return scheduledNodes;
}


Gex::ScheduleNodePtrList::iterator Gex::FindScheduledNode(
        Gex::ScheduleNodePtrList scheduled,
        const Gex::NodePtr& node)
{
    auto pred = [node](const ScheduledNodePtr& n){return (*n == node);};

    return std::find_if(scheduled.begin(), scheduled.end(), pred);
}


Gex::ScheduleNodePtrList Gex::SubScheduledNodes(const ScheduleNodePtrList& list, const Gex::NodePtr& node)
{
    ScheduleNodePtrList subGraph;

    auto pred = [node](const ScheduledNodePtr& s)
    {
        return (s->node && s->node == node);
    };

    // Make a full copy of the list.
    ScheduleNodePtrList workList = CopyScheduledNodes(list);

    // Find iterator at where our start node is placed.
    auto iter = std::find_if(workList.begin(), workList.end(), pred);
    if (iter == workList.end())
        return subGraph;

    std::set<ScheduledNodePtr> search = {*iter};

    subGraph.push_back(*iter);

    // Fill sub graph with scheduled node copies.
    for (;iter != workList.end(); iter++)
    {
        auto currentNode = (*iter);
        auto previousNodes = currentNode->previousNodes;

        ScheduleNodePtrList newPreviousNodes;
        for (const auto& previousNode : previousNodes)
        {
            if (search.find(previousNode) != search.end())
            {
                newPreviousNodes.push_back(previousNode);
            }
        }

        currentNode->previousNodes = newPreviousNodes;
        subGraph.push_back(currentNode);
        search.insert(currentNode);
    }

    return subGraph;
}

