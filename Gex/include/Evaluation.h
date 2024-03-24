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
        ScheduledNode* node = nullptr;
        NodeEvaluator* _evaluator = nullptr;
        bool stop = false;
        std::function<void(Node*)> nodeStart=nullptr;
        std::function<void(Node*, bool)> nodeEnd=nullptr;
        unsigned int index;

    public:
        EvaluatorThread(NodeEvaluator* evaluator, unsigned int index,
                        std::function<void(Node*)> onNodeStart=nullptr,
                        std::function<void(Node*, bool)> onNodeEnd=nullptr);

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
        int numberOfThreads;
        int runningThreads;
        std::vector<EvaluatorThread*> threads;
        std::vector<ScheduledNode*> schelNodes;
        GraphContext context;
        Profiler profiler;
        bool detached;
        int n = -1;

        EvaluationStatus status = EvaluationStatus::Preparing;
        std::function<void(Node*)> evalStart = nullptr;
        std::function<void(Node*, bool)> evalEnd = nullptr;
        std::function<void(const GraphContext&)> postEval = nullptr;

    protected:
        static void StartEvalThread(EvaluatorThread* thread);

    public:
        NodeEvaluator(ScheduleNodeList nodes, GraphContext& context,
                      Gex::Profiler profiler, bool detached=false,
                      unsigned int threads=1,
                      std::function<void(Node*)> onNodeStart=nullptr,
                      std::function<void(Node*, bool)> onNodeEnd=nullptr,
                      std::function<void(const GraphContext&)> postEvaluation=nullptr);

        ~NodeEvaluator();

        void Run();

        ScheduledNode* NextNode();

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
