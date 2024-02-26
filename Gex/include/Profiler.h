#ifndef GEX_PROFILER_H
#define GEX_PROFILER_H

#include "api.h"

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
    public:
        std::string category;
        std::string name;
    private:
        Time start;
        Time end;
        bool started;

    public:
        void Start();

        void Stop();

        Duration Duration() const;

        bool Running() const;

        Time StartTime() const;

        Time EndTime() const;
    };


    class EvaluationNodeProfiler;


    typedef std::map<std::string, std::vector<Event>> Profile;


    class GEX_API EvaluationProfiler
    {
        std::vector<Event> events;
        Time start;
        Time end;

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
    };


    typedef std::shared_ptr<EvaluationProfiler> Profiler;

    class GEX_API EvaluationNodeProfiler
    {
        unsigned int thread;
        Profiler profiler;
        std::string node;
    public:
        EvaluationNodeProfiler(Profiler profiler, Node* node, unsigned int thread);

        EvaluationNodeProfiler(const EvaluationNodeProfiler& other);

        unsigned int StartEvent(const std::string& name);

        void StopEvent(unsigned int index);

        Profiler GetProfiler() const;
    };

    typedef EvaluationNodeProfiler NodeProfiler;

    inline Profiler MakeProfiler()
    {
        return std::make_shared<EvaluationProfiler>();
    }

}

#define PROFILE(node, profiler, name, code) \
    auto name = profiler->CreateStep(node, #name);      \
    name->Start();                     \
    code;                             \
    name->Stop();

#endif //GEX_PROFILER_H
