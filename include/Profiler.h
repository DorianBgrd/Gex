#ifndef GEX_PROFILER_H
#define GEX_PROFILER_H

#include "api.h"

#include <chrono>
#include <string>
#include <vector>
#include <memory>

namespace Gex
{

    typedef std::chrono::time_point<std::chrono::high_resolution_clock> time;
    typedef std::chrono::nanoseconds duration;

    class Node;

    class GEX_API ProfileStep
    {
    private:
         time startpoint;
         time endpoint;
         std::string stepname;
         const Node* node;

    public:
        ProfileStep(const Node* node, const std::string& name);

        duration Duration() const;

        void Start();

        void Stop();

        time StartTime() const;

        time EndTime() const;

        std::string Name() const;

        const Node* Node() const;
    };


    typedef std::shared_ptr<ProfileStep> Step;
    typedef std::vector<std::shared_ptr<ProfileStep>> StepList;

    class Profiler;
    typedef std::shared_ptr<Profiler> ProfilerPtr;


    struct ProfileInfo
    {
        std::string path;
        unsigned int thread;
    };

    typedef std::vector<ProfileInfo> ProfileInfos;

    class GEX_API Profiler
    {
    private:

        StepList steps;
        time startpoint;
        time endpoint;
        ProfileInfos infos;

    public:
        Profiler() = default;

        Profiler(ProfilerPtr other);

        void Start();

        void Stop();

        duration Duration() const;

        Step CreateStep(const Node* node, const std::string& name);

        StepList GetSteps() const;

        time StartTime() const;

        time EndTime() const;

        void SetNodeThread(Gex::Node* node, unsigned int thread);

        unsigned int NodeThread(const std::string& node) const;

        unsigned int NodeThread(Gex::Node* node) const;

        unsigned int NumberOfThreads() const;
    };
}

#define PROFILE(node, profiler, name, code) \
    auto name = profiler->CreateStep(node, #name);      \
    name->Start();                     \
    code;                             \
    name->Stop();

#endif //GEX_PROFILER_H
