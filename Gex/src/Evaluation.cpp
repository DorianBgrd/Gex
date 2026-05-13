#include "Gex/include/Gex/Evaluation.h"
#include "Gex/include/Gex/Node.h"
#include "Gex/include/Gex/Scheduling.h"
#include <thread>
#include <mutex>


Gex::EvaluatorThread::EvaluatorThread(
        Gex::NodeEvaluator* eval, unsigned int index_,
        const ScheduleItemCallback& onNodeStart,
        const ScheduleItemSuccessCallback& onNodeEnd
)
{
    name = "Thread " + std::to_string(index_);
    _evaluator = eval;
    itemStart = onNodeStart;
    itemEnd = onNodeEnd;
}


void Gex::EvaluatorThread::Start()
{
    stop = false;

    Loop();
}


std::mutex nodeEvaluatorLock;


bool Gex::EvaluatorThread::AcquireNode()
{
    nodeEvaluatorLock.lock();

    func = _evaluator->Acquire();

    nodeEvaluatorLock.unlock();

    return func;
}


bool Gex::EvaluatorThread::ComputeNode()
{
    if (!func)
        return false;

    if (itemStart)
    {
        itemStart(func.Item());
    }

    bool result = func(
            _evaluator->Context(),
            _evaluator->GetProfiler(),
            name
    );

    if (itemEnd)
    {
        itemEnd(func.Item(), result);
    }

    return result;
}


void Gex::EvaluatorThread::Loop()
{
    while(!_evaluator->Done() && !stop)
    {
        auto an = _evaluator->profiler->StartEvent(
                name, "AcquireNode"
        );

        bool acquired = AcquireNode();

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


Gex::NodeEvaluator::NodeEvaluator(const ScheduledItemPtr& scheduled, GraphContext& ctx,
                                  const Profiler& profiler_, bool detached_,
                                  unsigned int threads_,
                                  const ScheduleItemCallback& onNodeStarted,
                                  const ScheduleItemSuccessCallback& onNodeDone,
                                  const std::function<void(const GraphContext&)>& postEvaluation):
                                  context(ctx), scheduled(scheduled)
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
    profiler->StopEvent(schedule);
}


void Gex::NodeEvaluator::Reset()
{
    n = -1;
//    for (const auto& schelNode: schelNodes)
//    {
//        schelNode->evaluated = false;
//    }

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
            auto nodeThread = std::make_shared<EvaluatorThread>(
                    this, i, evalStart, evalEnd
            );

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


Gex::EvalFunction Gex::NodeEvaluator::Acquire()
{
    return scheduled->Acquire().func;
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
