#ifndef GEX_PROFILER_H
#define GEX_PROFILER_H

#include "api.h"
#include "defs.h"

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Gex
{
    class Node;

    typedef std::chrono::time_point<std::chrono::high_resolution_clock> Time;
    typedef std::chrono::nanoseconds Duration;


    struct GEX_API Event
    {
        static bool registered;
    public:
        std::string category;
        std::string name;
    private:
        Time start;
        Time end;
        bool started = false;

    public:

        void Start();

        void Stop();

        Duration Duration() const;

        bool Running() const;

        Time StartTime() const;

        Time EndTime() const;

        bool operator==(const Event& other) const;

        static void RegisterPythonWrapper();
    };


    class EvaluationNodeProfiler;


    typedef std::map<std::string, std::vector<Event>> Profile;


    class GEX_API EvaluationProfiler:
            public std::enable_shared_from_this<EvaluationProfiler>
    {
        std::vector<Event> events;
        Time start;
        Time end;
        static bool registered;

    public:
        EvaluationProfiler() = default;

    protected:
        unsigned int StartEvent(unsigned int thread,
                                const std::string& category,
                                const std::string& name);

    public:
        unsigned int StartEvent(const std::string& category,
                                const std::string& name);

        void StopEvent(unsigned int index);

        void Start();

        void Stop();

        Time StartTime() const;

        Time EndTime() const;

        Duration Duration() const;

        Profile Result() const;

        void Reset();

        static void RegisterPythonWrapper();
    };


    typedef std::shared_ptr<EvaluationProfiler> Profiler;

    class GEX_API EvaluationNodeProfiler
    {
        static bool registered;

        Profiler profiler;
        std::string thread;
        std::string node;

    public:
        EvaluationNodeProfiler(const Profiler& profiler,
                               const NodePtr& node,
                               const std::string& thread);

        EvaluationNodeProfiler(const EvaluationNodeProfiler& other);

        unsigned int StartEvent(const std::string& name);

        void StopEvent(unsigned int index);

        Profiler GetProfiler() const;
    };

    inline Profiler MakeProfiler()
    {
        return std::make_shared<EvaluationProfiler>();
    }

    class ProfilerScope
    {
        Profiler profiler;
        unsigned int event = 0;
    public:
        ProfilerScope(Profiler profiler,
                      const std::string& category,
                      const std::string& name);

        ~ProfilerScope();

        void Stop();
    };


    class NodeProfilerScope
    {
        NodeProfiler profiler;
        unsigned int event = 0;
    public:
        NodeProfilerScope(NodeProfiler profiler,
                          const std::string& name);

        ~NodeProfilerScope();

        void Stop();
    };

}

#define PROFILE(node, profiler, name, code) \
    auto name = profiler->CreateStep(node, #name);      \
    name->Start();                     \
    code;                             \
    name->Stop();

#endif //GEX_PROFILER_H
