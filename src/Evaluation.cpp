#include "include/Evaluation.h"
#include "include/Node.h"
#include <thread>
#include <mutex>


Gex::EvaluatorThread::EvaluatorThread(Gex::NodeEvaluator* eval,
                                                  std::function<void(Node*)> onNodeStart,
                                                  std::function<void(Node*, bool)> onNodeEnd)
{
    _evaluator = eval;
    nodeStart = onNodeStart;
    nodeEnd = onNodeEnd;
}


void Gex::EvaluatorThread::Start()
{
    stop = false;

    Loop();
}


bool Gex::EvaluatorThread::AcquireNode()
{
    node = _evaluator->NextNode();

    return (node != nullptr);
}


bool Gex::EvaluatorThread::ComputeNode()
{
    while (!node->ShouldBeEvaluated())
    {
        std::this_thread::yield();
    }

    if (nodeStart)
    {
        nodeStart(node->node);
    }

    bool result = node->Compute(_evaluator->Context());

    if (nodeEnd)
    {
        nodeEnd(node->node, result);
    }

    return result;
}


std::mutex nodeEvaluatorLock;


void Gex::EvaluatorThread::Loop()
{
    while(!_evaluator->Done() && !stop)
    {
        nodeEvaluatorLock.lock();

        bool acquired = AcquireNode();

        nodeEvaluatorLock.unlock();

        if (!acquired)
        {
            break;
        }

        // If a node does not succeed its evaluation,
        // stop the whole process.
        bool success = ComputeNode();
        if (!success)
        {
            _evaluator->Terminate();
        }
    }

    _evaluator->ThreadsStopped();
}


void Gex::EvaluatorThread::Terminate()
{
    stop = true;
}






void Gex::NodeEvaluator::StartEvalThread(EvaluatorThread* thread)
{
    thread->Start();
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
        scheduledNode = new Gex::ScheduledNode();
        scheduledNode->node = node;
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


std::vector<Gex::ScheduledNode*> Gex::ScheduleNodes(std::vector<Gex::Node*> nodes)
{
    std::map<Gex::Node*, ScheduledNode*> schelNodes;

    std::vector<Gex::ScheduledNode*> roots;
    for (auto* node : nodes)
    {
        auto* schel = new ScheduledNode();
        schel->node = node;
        schelNodes[node] = schel;
    }

    for (auto sn: schelNodes)
    {
        auto sources = sn.first->UpstreamNodes();
        auto destinations = sn.first->DownstreamNodes();

        if (sources.empty())
        {
            roots.push_back(sn.second);
        }

        for (auto src : sources)
        {
            if (schelNodes.find(src) == schelNodes.end())
            {
                continue;
            }

            auto* scheclSrc = schelNodes.at(src);
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
    for (auto npl : dict)
    {
        for (auto np : npl.second)
        {
            scheduledNodes.push_back(np);
        }
    }

    return scheduledNodes;
}



Gex::NodeEvaluator::NodeEvaluator(std::vector<Node*> nodes_, GraphContext ctx, bool detached,
                                              std::function<void(Node*)> onNodeStarted,
                                              std::function<void(Node*, bool)> onNodeDone,
                                              std::function<void(const GraphContext&)> postEvaluation)
{
    context = ctx;
    postEval = postEvaluation;
    evalStart = onNodeStarted;
    evalEnd = onNodeDone;
    runningThreads = 0;
    schelNodes = ScheduleNodes(nodes_);
    numberOfThreads = 1;

    status = NodeEvaluator::EvaluationStatus::Running;
    std::vector<std::thread> stdthreads;
    for (unsigned int i = 0; i < numberOfThreads; i++)
    {
        auto* nodeThread = new EvaluatorThread(this, evalStart, evalEnd);

        threads.push_back(nodeThread);

        auto th = std::thread(NodeEvaluator::StartEvalThread, nodeThread);
        stdthreads.push_back(std::move(th));

        runningThreads +=1;
    }

    if(!detached)
    {
        for (unsigned int i = 0; i < stdthreads.size(); i++)
        {
            stdthreads.at(i).join();
        }
    }
}


Gex::ScheduledNode* Gex::NodeEvaluator::NextNode()
{
    n += 1;
    if (n >= schelNodes.size())
    {
        return nullptr;
    }

    return schelNodes.at(n);
}


Gex::GraphContext & Gex::NodeEvaluator::Context()
{
    return context;
}


void Gex::NodeEvaluator::End()
{
    if (postEval)
    {
        postEval(context);
    }

    status = EvaluationStatus::Done;
}


void Gex::NodeEvaluator::ThreadsStopped()
{
    runningThreads -= 1;
    if (runningThreads <= 0)
    {
        End();
    }
}


void Gex::NodeEvaluator::Terminate()
{
    for (EvaluatorThread* thread : threads)
    {
        thread->Terminate();
    }

    status = EvaluationStatus::Failed;
}


void Gex::NodeEvaluator::Stop()
{
    for (EvaluatorThread* thread : threads)
    {
        thread->Terminate();
    }

    status = EvaluationStatus::Canceled;
}


bool Gex::NodeEvaluator::Done() const
{
    if (status == EvaluationStatus::Preparing ||
        status == EvaluationStatus::Running)
    {
        return false;
    }

    return true;
}


Gex::NodeEvaluator::EvaluationStatus Gex::NodeEvaluator::Status() const
{
    return status;
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

bool Gex::ScheduledNode::Compute(Gex::GraphContext &context)
{
    bool result = node->Compute(context);
    evaluated = true;
    return result;
}

bool Gex::ScheduledNode::Evaluated() const
{
    return evaluated;
}
