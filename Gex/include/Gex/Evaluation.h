#ifndef NODELIBRARY2COMPLETE_EVALUATION_H
#define NODELIBRARY2COMPLETE_EVALUATION_H

#include "api.h"
#include "Scheduling.h"
#include "Profiler.h"
#include "Profiler.h"
#include "Node.h"

#include <vector>
#include <string>
#include <functional>


namespace Gex
{
    class Node;
    class NodeEvaluator;

    class EvaluatorThread
    {
        ScheduledNodePtr node = nullptr;
        NodeEvaluator* _evaluator = nullptr;
        bool stop = false;
        std::function<void(const NodePtr&)> nodeStart=nullptr;
        std::function<void(const NodePtr&, bool)> nodeEnd=nullptr;
        std::string name;

    public:
        EvaluatorThread(NodeEvaluator* evaluator, unsigned int index,
                        const std::function<void(const NodePtr&)>& onNodeStart=nullptr,
                        const std::function<void(const NodePtr&, bool)>& onNodeEnd=nullptr);

        void Start();

        bool AcquireNode();

        bool ComputeNode();

        void Loop();

        void Terminate();
    };


    typedef void(*Callback)(const GraphContext& ctx);


    class GEX_API NodeEvaluator
    {
        friend EvaluatorThread;

    public:
        enum class EvaluationStatus
        {
            Preparing,
            Ready,
            Running,
            Done,
            Failed,
            Canceled
        };

    private:
        unsigned int numberOfThreads;
        unsigned int runningThreads;
        EvaluatorThreadPtrList threads;
        ScheduleNodePtrList schelNodes;
        GraphContext context;
        Profiler profiler;
        bool detached;
        int n = -1;

        EvaluationStatus status = EvaluationStatus::Preparing;
        std::function<void(const NodePtr&)> evalStart;
        std::function<void(const NodePtr&, bool)> evalEnd;
        std::function<void(const GraphContext&)> postEval;

    public:
        NodeEvaluator(const ScheduleNodePtrList& nodes, GraphContext& context,
                      const Gex::Profiler& profiler, bool detached=false,
                      unsigned int threads=1,
                      const std::function<void(const NodePtr&)>& onNodeStart=nullptr,
                      const std::function<void(const NodePtr&, bool)>& onNodeEnd=nullptr,
                      const std::function<void(const GraphContext&)>& postEvaluation=nullptr);

        ~NodeEvaluator();

        void Reset();

        void Run();

        ScheduledNodePtr NextNode();

        GraphContext& Context();

        Profiler GetProfiler();

    protected:
        void ThreadsStopped();

        void Terminate();

        void End();

    public:
        /**
         * Stops the evaluation.
         * Note : This function can only be used while Evaluator
         * is detached
         */
        void Stop();

        bool Done() const;

        EvaluationStatus Status() const;
    };

}

#endif //NODELIBRARY2COMPLETE_EVALUATION_H
