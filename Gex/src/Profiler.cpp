#include "Gex/include/Profiler.h"
#include "Gex/include/Node.h"

#include <mutex>


void Gex::Event::Start()
{
    started = true;
    start = std::chrono::high_resolution_clock::now();
}


void Gex::Event::Stop()
{
    end = std::chrono::high_resolution_clock::now();
    started = false;
}


Gex::Duration Gex::Event::Duration() const
{
    return std::chrono::duration_cast<Gex::Duration>(end - start);
}


bool Gex::Event::Running() const
{
    return started;
}


Gex::Time Gex::Event::StartTime() const
{
    return start;
}


Gex::Time Gex::Event::EndTime() const
{
    return end;
}


bool Gex::Event::operator==(const Event& other) const
{
    return (category == other.category &&
            name == other.name &&
            start == other.start &&
            end == other.end);
}


std::mutex profileMutex;


unsigned int Gex::EvaluationProfiler::StartEvent(unsigned int thread,
                                                 const std::string& category,
                                                 const std::string& name)
{
    profileMutex.lock();

    Event event;
    event.category = category;
    event.name = name;

    event.Start();

    unsigned int index = 0;
    if (!events.empty())
        index = events.size();

    events.push_back(event);

    profileMutex.unlock();
    return index;
}


unsigned int Gex::EvaluationProfiler::StartEvent(const std::string& category,
                                                 const std::string& name)
{
    return StartEvent(0, category, name);
}


void Gex::EvaluationProfiler::StopEvent(unsigned int index)
{
    events[index].Stop();
}


void Gex::EvaluationProfiler::Start()
{
    start = std::chrono::high_resolution_clock::now();
}

void Gex::EvaluationProfiler::Stop()
{
    for (auto event : events)
    {
        if (event.Running())
            event.Stop();
    }

    end = std::chrono::high_resolution_clock::now();
}


Gex::Time Gex::EvaluationProfiler::StartTime() const
{
    return start;
}


Gex::Time Gex::EvaluationProfiler::EndTime() const
{
    return end;
}


Gex::Duration Gex::EvaluationProfiler::Duration() const
{
    size_t dur = (end - start).count();
    return std::chrono::duration_cast<Gex::Duration>(end - start);
}


Gex::Profile Gex::EvaluationProfiler::Result() const
{
    Profile result;

    for (const auto& event : events)
    {
        if (result.count(event.category) == 0)
        {
            result[event.category] = std::vector<Event>();
        }

        result.at(event.category).push_back(event);
    }

    return result;
}


void Gex::EvaluationProfiler::Reset()
{
    events.clear();
}


Gex::EvaluationNodeProfiler::EvaluationNodeProfiler(
        const Profiler& profiler_,
        const NodePtr& node_,
        const std::string& thread_
)
{
    node = node_->Path();
    profiler = profiler_;
    thread = thread_;
}


Gex::EvaluationNodeProfiler::EvaluationNodeProfiler(
        const EvaluationNodeProfiler& other)
{
    node = other.node;
    profiler = other.profiler;
    thread = other.thread;
}

unsigned int Gex::EvaluationNodeProfiler::StartEvent(const std::string& name)
{
    return profiler->StartEvent(thread, node + "::" + name);
}


void Gex::EvaluationNodeProfiler::StopEvent(unsigned int index)
{
    profiler->StopEvent(index);
}


Gex::Profiler Gex::EvaluationNodeProfiler::GetProfiler() const
{
    return profiler;
}


Gex::ProfilerScope::ProfilerScope(Profiler profiler_,
                                  const std::string& category,
                                  const std::string& name)
{
    profiler = profiler_;
    event = profiler_->StartEvent(category, name);
}


Gex::ProfilerScope::~ProfilerScope()
{
    Stop();
}


void Gex::ProfilerScope::Stop()
{
    profiler->StopEvent(event);
}


Gex::NodeProfilerScope::NodeProfilerScope(NodeProfiler profiler_,
                                          const std::string& name):
                                          profiler(profiler_)
{
    event = profiler.StartEvent(name);
}


Gex::NodeProfilerScope::~NodeProfilerScope()
{
    Stop();
}


void Gex::NodeProfilerScope::Stop()
{
    profiler.StopEvent(event);
}