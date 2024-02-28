#include "Gex/include/Evaluation.h"
#include "Gex/include/Node.h"
#include <thread>
#include <mutex>


Gex::EvaluatorThread::EvaluatorThread(Gex::NodeEvaluator* eval, unsigned int index_,
                                      std::function<void(Node*)> onNodeStart,
                                      std::function<void(Node*, bool)> onNodeEnd)
{
    index = index_;
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
    auto nodeProfiler = NodeProfiler(_evaluator->GetProfiler(), node->node, index);

    auto wn = nodeProfiler.StartEvent("Waiting previous nodes");
    while (!node->ShouldBeEvaluated())
    {
        std::this_thread::yield();
    }
    nodeProfiler.StopEvent(wn);

    if (nodeStart)
    {
        nodeStart(node->node);
    }

    bool result = node->Compute(_evaluator->Context(), nodeProfiler);

    if (nodeEnd)
    {
        auto ne = nodeProfiler.StartEvent("NodeEnd");

        nodeEnd(node->node, result);

        nodeProfiler.StopEvent(ne);
    }

    return result;
}


std::mutex nodeEvaluatorLock;


void Gex::EvaluatorThread::Loop()
{
    while(!_evaluator->Done() && !stop)
    {
        auto an = _evaluator->profiler->StartEvent("Thread " + std::to_string(index),
                                                              "AcquireNode");
        nodeEvaluatorLock.lock();

        bool acquired = AcquireNode();

        nodeEvaluatorLock.unlock();
        _evaluator->profiler->StopEvent(an);

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


std::vector<Gex::ScheduledNode*> Gex::ScheduleNodes(std::vector<Gex::Node*> nodes)
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
            if (n->node->IsCompound())
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



void StartThread(Gex::EvaluatorThread* th)
{
    return th->Start();
}


Gex::NodeEvaluator::NodeEvaluator(std::vector<Node*> nodes_, GraphContext& ctx,
                                  Profiler profiler_, bool detached_,
                                  unsigned int threads_,
                                  std::function<void(Node*)> onNodeStarted,
                                  std::function<void(Node*, bool)> onNodeDone,
                                  std::function<void(const GraphContext&)> postEvaluation)
{
    profiler = profiler_;
    unsigned int init = profiler->StartEvent("Prepare", "Init evaluator");

    detached = detached_;
    context = ctx;
    postEval = postEvaluation;
    evalStart = onNodeStarted;
    evalEnd = onNodeDone;
    runningThreads = 0;
    numberOfThreads = threads_;

    status = NodeEvaluator::EvaluationStatus::Running;
    profiler->StopEvent(init);

    unsigned int schedule = profiler->StartEvent("Prepare", "Schedule");
    schelNodes = ScheduleNodes(nodes_);
    profiler->StopEvent(schedule);
}


void Gex::NodeEvaluator::Run()
{
    std::vector<std::thread> stdthreads;

    if (numberOfThreads > 1 || detached)
    {
        unsigned int threadStart = profiler->StartEvent("Prepare", "Starting threads");
        for (unsigned int i = 0; i < numberOfThreads; i++)
        {
            auto* nodeThread = new EvaluatorThread(this, i, evalStart, evalEnd);

            threads.push_back(nodeThread);

            auto th = std::thread(&StartThread, nodeThread);
            stdthreads.push_back(std::move(th));

            runningThreads +=1;
        }
        profiler->StopEvent(threadStart);

        if(!detached)
        {
            for (auto& stdthread : stdthreads)
            {
                stdthread.join();
            }
        }
    }
    else
    {
        EvaluatorThread th(this, 0, evalStart, evalEnd);
        th.Start();
    }
}


Gex::NodeEvaluator::~NodeEvaluator()
{
    for (auto* thread : threads)
    {
        delete thread;
    }

    threads.clear();
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


Gex::Profiler Gex::NodeEvaluator::GetProfiler()
{
    return profiler;
}


void Gex::NodeEvaluator::End()
{
    if (postEval)
    {
        postEval(context);
    }

    status = EvaluationStatus::Done;
    profiler->Stop();
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

    profiler->Stop();
}


void Gex::NodeEvaluator::Stop()
{
    for (EvaluatorThread* thread : threads)
    {
        thread->Terminate();
    }

    status = EvaluationStatus::Canceled;

    profiler->Stop();
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


Gex::ScheduledNode::ScheduledNode(Node* node_)
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
    return node->Compute(context, profiler);
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
