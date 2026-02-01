#include "Gex/include/Commands.h"

#include "Gex/include/io.h"

#include "boost/python.hpp"


bool Gex::RunGraph(const NodePtr& graph,
                   const AttributeValues& values,
                   int threadNumber,
                   const Profiler& profiler,
                   Feedback* result)
{
    std::vector<std::string> warningMessages;
    for (const auto& value : values)
    {
        if (auto attr = graph->GetAttribute(value.first))
        {
            if (!attr->SetAnyValue(value.second))
            {
                warningMessages.push_back(
                    "Could not set attribute "
                    + value.first + " value."
                );
            }
        }
    }

    bool execution = graph->Run(profiler, threadNumber);

    std::string message;
    if (!warningMessages.empty() && result)
    {
        message = "# WARNING :";
        for (const auto& m : warningMessages)
        {
            message += m + "\n";
        }

        result->Set(Status::Warning, message);
    }

    if (!execution)
    {
        message += "# FAILURE : Execution failed.";
    }

    if (!message.empty())
    {
        std::cout << message << std::endl;
    }

    return execution;
}

bool Gex::RunGraph(const NodePtr& graph,
                   int threadNumber,
                   const Profiler& profiler,
                   Feedback* result)
{
    return RunGraph(graph, {}, threadNumber,
                    profiler, result);
}


bool Gex::LoadAndRunGraph(const std::string& filepath,
                          const AttributeValues& values,
                          int threadNumber,
                          const Profiler& profiler,
                          Feedback* result)
{
    NodePtr graph = LoadGraph(filepath, result);

    if (!result)
    {
        return false;
    }

    return RunGraph(graph, values, threadNumber,
                    profiler, result);
}

bool Gex::LoadAndRunGraph(const std::string& filepath,
                          int threadNumber,
                          const Profiler& profiler,
                          Feedback* result)
{
    return LoadAndRunGraph(filepath, {}, threadNumber,
                           profiler, result);
}


static bool registered = false;


boost::python::object Python_RunGraph(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::NodePtr graph = boost::python::extract<Gex::NodePtr>(args[0]);

    boost::python::dict attrValues;

    int length = boost::python::len(args);

    if (length > 1)
    {
        attrValues = boost::python::dict(args[1]);
    }
    else
    {
        attrValues = boost::python::dict(
                kwargs.get(
                        "attributeValues",
                        boost::python::dict()
                )
        );
    }

    int threadNumber = 1;
    if (length > 2)
    {
        threadNumber = boost::python::extract<int>(args[2]);
    }
    else
    {
        auto thn = kwargs.get("threadNumber");
        if (thn)
        {
            threadNumber = boost::python::extract<int>(thn);
        }
    }

    Gex::Profiler profiler;
    if (length > 3)
    {
        profiler = boost::python::extract<Gex::Profiler>(args[3]);
    }
    else
    {
        auto thn = kwargs.get("threadNumber");
        if (thn)
        {
            profiler = boost::python::extract<Gex::Profiler>(thn);
        }
    }

    return boost::python::object(
            Gex::RunGraph(graph, threadNumber, profiler)
    );
}




boost::python::object Python_LoadAndRunGraph(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    return {};
}


void Gex::Python::Commands::RegisterPythonWrapper()
{
    if (registered)
    {
        return;
    }

//    boost::python::def()

    registered = true;
}
