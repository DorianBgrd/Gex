#ifndef GEX_COMMANDS
#define GEX_COMMANDS

#include "Node.h"

namespace Gex
{
    typedef std::unordered_map<std::string, std::any> AttributeValues;

    bool RunGraph(const NodePtr& graph, const AttributeValues& values,
                  int threadNumber=1, const Profiler& profiler=nullptr,
                  Feedback* result=nullptr);

    bool RunGraph(const NodePtr& graph, int threadNumber=1,
                  const Profiler& profiler=nullptr,
                  Feedback* result=nullptr);

    bool LoadAndRunGraph(const std::string& filepath,
                         const AttributeValues& values,
                         int threadNumber=1,
                         const Profiler& profiler=nullptr,
                         Feedback* result=nullptr);

    bool LoadAndRunGraph(const std::string& filepath,
                         int threadNumber=1,
                         const Profiler& profiler=nullptr,
                         Feedback* result=nullptr);

    namespace Python::Commands
    {
        void RegisterPythonWrapper();
    }
}

#endif