#include "include/Profiler.h"
#include "include/Node.h"


Gex::ProfileStep::ProfileStep(const Gex::Node* node_,
                              const std::string& name)
{
    node = node_;
    stepname = name;
}


Gex::duration Gex::ProfileStep::Duration() const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(endpoint - startpoint);
}


void Gex::ProfileStep::Start()
{
    startpoint = std::chrono::high_resolution_clock::now();
}


void Gex::ProfileStep::Stop()
{
    endpoint = std::chrono::high_resolution_clock::now();
}


Gex::time Gex::ProfileStep::StartTime() const
{
    return startpoint;
}


Gex::time Gex::ProfileStep::EndTime() const
{
    return endpoint;
}


std::string Gex::ProfileStep::Name() const
{
    return (node->Name() + " - " + stepname);
}


const Gex::Node* Gex::ProfileStep::Node() const
{
    return node;
}


Gex::Profiler::Profiler(ProfilerPtr other)
{
    steps = other->steps;
}


void Gex::Profiler::Start()
{
    startpoint = std::chrono::high_resolution_clock::now();
}


void Gex::Profiler::Stop()
{
    endpoint = std::chrono::high_resolution_clock::now();
}


Gex::duration Gex::Profiler::Duration() const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(endpoint - startpoint);
}


Gex::Step Gex::Profiler::CreateStep(const Node* node,
                                    const std::string& name)
{
    Step step = std::make_shared<ProfileStep>(node, name);
    steps.push_back(step);
    return step;
}


Gex::StepList Gex::Profiler::GetSteps() const
{
    return steps;
}


Gex::time Gex::Profiler::StartTime() const
{
    return startpoint;
}


Gex::time Gex::Profiler::EndTime() const
{
    return endpoint;
}


void Gex::Profiler::SetNodeThread(Gex::Node* node, unsigned int thread)
{
    ProfileInfo info = {node->Path(), thread};
    infos.push_back(info);
}


unsigned int Gex::Profiler::NodeThread(const std::string& path) const
{
    for (auto info : infos)
    {
        if (info.path == path)
        {
            return info.thread;
        }
    }

    return 0;
}


struct Pred
{
    Gex::ProfileInfo operator()(Gex::ProfileInfo i1,
                                Gex::ProfileInfo i2)
    {
        return (i1.thread < i2.thread) ? i2 : i1;
    }
};


unsigned int Gex::Profiler::NodeThread(Gex::Node* node) const
{
    return NodeThread(node->Path());
}


unsigned int Gex::Profiler::NumberOfThreads() const
{
    unsigned int max = 0;
    for (const auto& info : infos)
    {
        if (info.thread < max)
            continue;
        max = info.thread;
    }
    return max + 1;  // Ids starts at 0.
}