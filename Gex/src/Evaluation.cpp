#include "Gex/include/Evaluation.h"
#include "Gex/include/Node.h"
#include "Gex/include/Scheduling.h"
#include <thread>
#include <mutex>


Gex::EvaluatorThread::EvaluatorThread(Gex::NodeEvaluator* eval, unsigned int index_,
                                      std::function<void(const NodePtr&)> onNodeStart,
                                      std::function<void(const NodePtr&, bool)> onNodeEnd)
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
    if (node->node.expired())
        return false;

    auto lockedNode = node->node.lock();

    auto nodeProfiler = NodeProfiler(_evaluator->GetProfiler(),
                                     lockedNode, index);

    auto te = nodeProfiler.StartEvent("Testing eval");
    bool evaluate = node->ShouldBeEvaluated();
    nodeProfiler.StopEvent(te);

    auto wn = nodeProfiler.StartEvent("Waiting previous nodes");
    while (!evaluate)
    {
        std::this_thread::yield();
    }
    nodeProfiler.StopEvent(wn);

    if (nodeStart)
    {
        nodeStart(lockedNode);
    }

    bool result = node->Compute(_evaluator->Context(), nodeProfiler);

    if (nodeEnd)
    {
        auto ne = nodeProfiler.StartEvent("NodeEnd");

        nodeEnd(lockedNode, result);

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






void StartThread(const Gex::EvaluatorThreadPtr& th)
{
    return th->Start();
}


Gex::NodeEvaluator::NodeEvaluator(const ScheduleNodePtrList& nodes, GraphContext& ctx,
                                  const Profiler& profiler_, bool detached_,
                                  unsigned int threads_,
                                  const std::function<void(const std::shared_ptr<Node> &)>& onNodeStarted,
                                  const std::function<void(const std::shared_ptr<Node> &, bool)>& onNodeDone,
                                  const std::function<void(const GraphContext&)>& postEvaluation):
                                  context(ctx)
{
    profiler = profiler_;
    unsigned int init = profiler->StartEvent("Prepare", "Init evaluator");

    detached = detached_;
    postEval = postEvaluation;
    evalStart = onNodeStarted;
    evalEnd = onNodeDone;
    runningThreads = 0;
    numberOfThreads = threads_;

    status = NodeEvaluator::EvaluationStatus::Ready;
    profiler->StopEvent(init);

    unsigned int schedule = profiler->StartEvent("Prepare", "Schedule");
    schelNodes = nodes;
    profiler->StopEvent(schedule);
}


void Gex::NodeEvaluator::Reset()
{
    n = -1;
    for (const auto& schelNode: schelNodes)
    {
        schelNode->evaluated = false;
    }

    runningThreads = 0;
    status = NodeEvaluator::EvaluationStatus::Running;
}


void Gex::NodeEvaluator::Run()
{
    status = NodeEvaluator::EvaluationStatus::Running;

    std::vector<std::thread> stdthreads;

    if (numberOfThreads > 1 || detached)
    {
        unsigned int threadStart = profiler->StartEvent("Prepare", "Starting threads");
        for (unsigned int i = 0; i < numberOfThreads; i++)
        {
            auto nodeThread = std::make_shared<EvaluatorThread>(this, i, evalStart, evalEnd);

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
        runningThreads = 1;

        th.Start();
    }
}


Gex::NodeEvaluator::~NodeEvaluator()
{
    threads.clear();
}


Gex::ScheduledNodePtr Gex::NodeEvaluator::NextNode()
{
    n += 1;
    if (n >= schelNodes.size())
    {
        return nullptr;
    }

    return schelNodes[n];
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
    for (const EvaluatorThreadPtr& thread : threads)
    {
        thread->Terminate();
    }

    status = EvaluationStatus::Failed;

    profiler->Stop();

    if (postEval)
    {
        postEval(context);
    }
}


void Gex::NodeEvaluator::Stop()
{
    for (const EvaluatorThreadPtr& thread : threads)
    {
        thread->Terminate();
    }

    status = EvaluationStatus::Canceled;

    profiler->Stop();

    if (postEval)
    {
        postEval(context);
    }
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
