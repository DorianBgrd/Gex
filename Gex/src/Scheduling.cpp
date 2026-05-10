#include "Gex/include/Gex/Scheduling.h"
#include "Gex/include/Gex/Node.h"

#include <map>
#include <set>




bool Gex::ScheduledItem::ShouldBeEvaluated() const
{
    for (const auto& prev : previous)
    {
        if (!prev->Evaluated())
        {
            return false;
        }
    }

    return true;
}


//bool Gex::ScheduledItem::Evaluate(GraphContext &context,
//                                  NodeProfiler &profiler)
//{
//    return true;
//}


//bool Gex::ScheduledItem::Compute(GraphContext &context,
//                                 NodeProfiler &profiler)
//{
//    bool result = Evaluate(context, profiler);
//    evaluated = true;
//
////    SignalEvaluated();
//
//    return result;
//}


bool Gex::ScheduledItem::Evaluated() const
{
    return evaluated;
}


Gex::CallbackId Gex::ScheduledItem::RegisterCallback(
        const ScheduleItemCallback& cb
)
{
    index++;
    callbacks[index] = cb;

    return index;
}


bool Gex::ScheduledItem::DeregisterCallback(CallbackId index)
{
    auto iter = callbacks.find(index);
    if (iter == callbacks.end())
    {
        return false;
    }

    callbacks.erase(iter);
    return true;
}


void Gex::ScheduledItem::LinkPrevious(const ScheduledItemWkPtr &prev)
{
    previous.push_back(prev);

    prev->LinkFuture(shared_from_this());
}


void Gex::ScheduledItem::LinkFuture(const ScheduledItemWkPtr &dest)
{
    next.push_back(dest);
}


void Gex::ScheduledItem::UnlinkPrevious(const ScheduledItemWkPtr &prev)
{
    auto iter = std::find(previous.begin(), previous.end(), prev);
    if (iter == previous.end())
    {
        return;
    }

    previous.erase(iter);

    prev->UnlinkFuture(shared_from_this());
}


void Gex::ScheduledItem::UnlinkFuture(const ScheduledItemWkPtr &dest)
{
    auto iter = std::find(next.begin(), next.end(), dest);
    if (iter == next.end())
    {
        return;
    }

    next.erase(iter);
}


bool Gex::ScheduledItem::Acquire(EvalFunction& funcCaller)
{
    while (!ShouldBeEvaluated())
    {

    }

    Evaluation function;
    evaluated = Advance(function);

    funcCaller.Configure(
            shared_from_this(),
            function
    );

    return evaluated;
}


bool Gex::ScheduledItem::Advance(Evaluation& function)
{
    return true;
}


std::vector<Gex::ScheduledItemPtr> Gex::ScheduledItem::Items() const
{
    return {};
}



Gex::ScheduledNode::ScheduledNode(const NodeWkPtr& node_)
{
    node = node_;
}


//bool Gex::ScheduledNode::Evaluate(GraphContext &context,
//                                  NodeProfiler &profiler)
//{
//    return node->Compute(context, profiler);
//}


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


Gex::NodeWkPtr Gex::ScheduledNode::GetNode() const
{
    return node;
}


bool Gex::ScheduledNode::Advance(Evaluation& function)
{
    function = [this](GraphContext& ctx, const Profiler& npf, const std::string& thread)
    {
        auto prof = EvaluationNodeProfiler(
                npf, node.ToShared(),
                thread
        );

        return node->Compute(ctx, prof);
    };

    return true;
}


Gex::ScheduledGroup::ScheduledGroup(
        const std::vector<ScheduledItemPtr>& nodes_
): items(nodes_)
{
    current = items.begin();
    end = items.end();
}


bool Gex::ScheduledGroup::Advance(
        EvalFunction& function
)
{
    if (current == end)
    {
        return true;
    }

    auto scheduled = (*current);

    if (scheduled->Advance(function))
    {
        current++;
    }

    return (current == end);
}


std::vector<Gex::ScheduledItemPtr> Gex::ScheduledGroup::Items() const
{
    return items;
}


typedef Gex::ScheduledNodePtr SchelNode;
typedef std::vector<Gex::ScheduledNodePtr> SchelNodeList;
typedef std::map<SchelNode, unsigned int> SchelNodeDict;
typedef std::map<unsigned int, SchelNodeList> SchelNodeSortedDict;


//void ResolveNodesLevels(const SchelNode& node, SchelNodeDict& nodes, unsigned int level)
//{
//    if (nodes.find(node) != nodes.end())
//    {
//        auto l = nodes.at(node);
//        if (level > l)
//        {
//            nodes[node] = level;
//        }
//    }
//    else
//    {
//        nodes[node] = level;
//    }
//
//    for (const auto& nextNode : node->futureNodes)
//    {
//        ResolveNodesLevels(nextNode.ToShared(), nodes, level + 1);
//    }
//}


//Gex::ScheduleNodePtrList Gex::CopyScheduledNodes(
//        const Gex::ScheduleNodePtrList& sources)
//{
//    std::unordered_map<Gex::NodePtr, Gex::ScheduledNodePtr> duplis;
//
//    Gex::ScheduleNodePtrList copiedGraph;
//    for (const auto& schelNode : sources)
//    {
//        if (!schelNode)
//            continue;
//
//        auto copy = std::make_shared<Gex::ScheduledNode>(
//                Gex::ScheduledNode(schelNode));
//        duplis[schelNode->node.lock()] = copy;
//
//        copiedGraph.push_back(copy);
//    }
//
//    for (auto& copy : copiedGraph)
//    {
//        Gex::ScheduleNodeWkPtrList newFuture;
//        for (const auto& future : copy->futureNodes)
//        {
//            auto match = duplis.find(future->node.lock());
//            if (match != duplis.end())
//                newFuture.emplace_back(match->second);
//            else
//                newFuture.push_back(future);
//        }
//        copy->futureNodes = newFuture;
//
//        Gex::ScheduleNodeWkPtrList newPrevious;
//        for (const auto& previous : copy->previousNodes)
//        {
//            auto match = duplis.find(previous->node.lock());
//            if (match != duplis.end())
//                newPrevious.emplace_back(match->second);
//            else
//                newPrevious.push_back(previous);
//        }
//        copy->previousNodes = newPrevious;
//    }
//
//    return copiedGraph;
//}
//
//
//Gex::ScheduleNodePtrList Gex::ScheduleNodes(const NodeList& nodes, bool expand)
//{
//    std::map<Gex::NodePtr, ScheduledNodePtr> schelNodes;
//
//    ScheduleNodePtrList roots;
//    for (const auto& node : nodes)
//    {
//        schelNodes[node] = node->ToScheduledNode();
//    }
//
//    for (const auto& sn: schelNodes)
//    {
//        auto sources = sn.first->UpstreamNodes();
//
//        if (sources.empty())
//        {
//            roots.push_back(sn.second);
//        }
//
//        for (const auto& src : sources)
//        {
//            if (!src)
//                continue;
//
//            auto indx = schelNodes.find(src.ToShared());
//            if (indx == schelNodes.end())
//            {
//                continue;
//            }
//
//            auto scheclSrc = indx->second;
//            sn.second->previousNodes.emplace_back(scheclSrc);
//            scheclSrc->futureNodes.emplace_back(sn.second);
//        }
//    }
//
//    SchelNodeDict nodesPerLevels;
//    for (const auto& snode : roots)
//    {
//        ResolveNodesLevels(snode, nodesPerLevels, 0);
//    }
//
//    SchelNodeSortedDict dict;
//    for (const auto& p : nodesPerLevels)
//    {
//        if (dict.find(p.second) == dict.end())
//        {
//            dict[p.second] = SchelNodeList();
//        }
//
//        dict.at(p.second).push_back(p.first);
//    }
//
//
//    std::vector<Gex::ScheduledNodePtr> scheduledNodes;
//    for (auto iter = dict.begin(); iter != dict.end(); iter++)
//    {
//        // For each node in scheduled nodes.
//        for (const auto& p : iter->second)
//        {
//            auto n = p;
//
//            if (!n->node)
//                continue;
//
//            NodePtr lockedNode = n->node.lock();
//            // If it is a compound, expand it.
//            if (expand && lockedNode->IsCompound())
//            {
//                // Transform the compound to a series of scheduled nodes.
//                auto cmpSchels = lockedNode->ToScheduledNodes();
//
//                if (!cmpSchels.empty()) // Indicate that this compound node does not expand.
//                {
//                    // Append resulting scheduled nodes from resulting scheduled.
//                    scheduledNodes.insert(scheduledNodes.end(), cmpSchels.begin(), cmpSchels.end());
//
//                    // Previous nodes of the first compound series node is the current
//                    // node previous nodes.
//                    cmpSchels.front()->previousNodes = n->previousNodes;
//
//                    // Then, for each previous nodes of current node, remove it from
//                    // their own future nodes and replace it with the first nodes from
//                    // compound series.
//                    for (const auto& src : n->previousNodes)
//                    {
//                        auto srcindex = std::remove(src->futureNodes.begin(), src->futureNodes.end(), n);
//                        if (srcindex == src->futureNodes.end())
//                            continue;
//
//                        src->futureNodes.emplace_back(cmpSchels.front());
//                    }
//
//                    // Then, for each future nodes of current node, remove it from
//                    // their own previous nodes and replace it with the last nodes
//                    // from compound series.
//                    cmpSchels.back()->futureNodes = n->futureNodes;
//                    for (const auto& src : n->futureNodes)
//                    {
//                        auto dstindex = std::remove(src->previousNodes.begin(), src->previousNodes.end(), n);
//                        if (dstindex == src->previousNodes.end())
//                            continue;
//
//                        src->previousNodes.emplace_back(cmpSchels.back());
//                    }
//                }
//
//                else
//                {
//                    scheduledNodes.push_back(n);
//                }
//            }
//            else
//            {
//                scheduledNodes.push_back(n);
//            }
//        }
//
//    }
//
//    return scheduledNodes;
//}
//
//
//Gex::ScheduleNodePtrList::iterator Gex::FindScheduledNode(
//        Gex::ScheduleNodePtrList scheduled,
//        const Gex::NodePtr& node)
//{
//    auto pred = [node](const ScheduledNodePtr& n){return (*n == node);};
//
//    return std::find_if(scheduled.begin(), scheduled.end(), pred);
//}
//
//
//Gex::ScheduleNodePtrList Gex::SubScheduledNodes(const ScheduleNodePtrList& list, const Gex::NodePtr& node)
//{
//    ScheduleNodePtrList subGraph;
//
//    auto pred = [node](const ScheduledNodePtr& s)
//    {
//        return (s->node && s->node == node);
//    };
//
//    // Make a full copy of the list.
//    ScheduleNodePtrList workList = CopyScheduledNodes(list);
//
//    // Find iterator at where our start node is placed.
//    auto iter = std::find_if(workList.begin(), workList.end(), pred);
//    if (iter == workList.end())
//        return subGraph;
//
//    std::set<ScheduledNodePtr> search = {*iter};
//
//    subGraph.push_back(*iter);
//
//    // Fill sub graph with scheduled node copies.
//    for (;iter != workList.end(); iter++)
//    {
//        auto currentNode = (*iter);
//        auto previousNodes = currentNode->previousNodes;
//
//        ScheduleNodeWkPtrList newPreviousNodes;
//        for (const auto& previousNode : previousNodes)
//        {
//            auto shared = previousNode.ToShared();
//            if (search.find(shared) != search.end())
//            {
//                newPreviousNodes.push_back(previousNode);
//            }
//        }
//
//        currentNode->previousNodes = newPreviousNodes;
//        subGraph.push_back(currentNode);
//        search.insert(currentNode);
//    }
//
//    return subGraph;
//}
//


struct GraphNode
{
    Gex::NodeWkPtr node;
    int index;

    bool operator==(const GraphNode& other) const
    {
        return node == other.node;
    }

    bool operator>(const GraphNode& other) const
    {
        return index > other.index;
    }

    bool operator<(const GraphNode& other) const
    {
        return index < other.index;
    }

    bool operator>=(const GraphNode& other) const
    {
        return index >= other.index;
    }

    bool operator<=(const GraphNode& other) const
    {
        return index <= other.index;
    }
};


typedef std::multiset<GraphNode, std::greater<>> GraphNodesList;
typedef std::map<Gex::NodeWkPtr, Gex::ScheduledItemPtr> ScheduledNodesMap;


void TraverseGraph(const Gex::NodeWkPtr& node,
                   int index, GraphNodesList& nodes)
{
    nodes.insert({node, index});

    for (const auto& upstreamNode : node->UpstreamNodes())
    {
        TraverseGraph(upstreamNode, index + 1, nodes);
    }
}


std::vector<Gex::CustomSchedulerBuilder> Gex::SchedulerRegistry::registry;
std::mutex Gex::SchedulerRegistry::lock;



Gex::ScheduledGroupPtr Gex::ScheduleGraph(const NodeList& nodes)
{
    // Find node without future.
    std::vector<Gex::NodePtr> lastNodes;

    for (const auto& node : nodes)
    {
        if (node->DownstreamNodes().empty())
        {
            lastNodes.push_back(node);
        }
    }

    std::vector<ScheduledItemPtr> resultNodes;

    GraphNodesList indexNodes;

    for (const auto& node : lastNodes)
    {
        TraverseGraph(node, 0, indexNodes);
    }

    ScheduledNodesMap schelMap;
    std::vector<ScheduledItemPtr> ptrs;
    for (const auto& graphNode : indexNodes)
    {
        auto ptr = graphNode.node->Schedule();
        schelMap.emplace(graphNode.node, ptr);
        ptrs.emplace_back(ptr);
    }

    for (const auto& nodeMap : schelMap)
    {
        for (const auto& upstream : nodeMap.first->UpstreamNodes())
        {
            nodeMap.second->LinkPrevious(schelMap.find(upstream)->second);
        }
    }

    return std::make_shared<ScheduledGroup>(ptrs);
}


Gex::Scheduler::Scheduler(
        const std::vector<CustomSchedulerPtr>& customSchedulers
): schedulers(customSchedulers)
{

}





Gex::ScheduledGroupPtr Gex::Scheduler::Schedule(
        const NodeList& nodes
) const
{
    auto defaultGroup = ScheduleGraph(nodes);

//    for (const auto& customScheduler : schedulers)
//    {
//        customScheduler->Schedule(defaultGroup);
//    }

    return defaultGroup;
}


Gex::Scheduler Gex::SchedulerRegistry::CreateScheduler()
{
    std::vector<CustomSchedulerPtr> schedulers;
    for (const auto& gen : registry)
    {
        schedulers.push_back(gen.func());
    }

    return {schedulers};
}
