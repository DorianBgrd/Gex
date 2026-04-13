#include "Gex/include/Commands.h"

#include "Gex/include/io.h"


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


pybind11::object Python_RunGraph(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::NodePtr graph = args[0].cast<Gex::NodePtr>();

    pybind11::dict attrValues;

    int length = pybind11::len(args);

    if (length > 1)
    {
        attrValues = pybind11::dict(args[1]);
    }
    else if (kwargs.contains("attributeValues"))
    {
        attrValues = pybind11::dict(
                kwargs["attributeValues"]
        );
    }

    int threadNumber = 1;
    if (length > 2)
    {
        threadNumber = args[2].cast<int>();
    }
    else
    {
        if (kwargs.contains("threadNumber"))
        {
            threadNumber = kwargs["threadNumber"].cast<int>();
        }
    }

    Gex::Profiler profiler;
    if (length > 3)
    {
        profiler = args[3].cast<Gex::Profiler>();
    }
    else
    {
        if (kwargs.contains("threadNumber"))
        {
            profiler = kwargs["threadNumber"].cast<Gex::Profiler>();
        }
    }

    return pybind11::cast(
            Gex::RunGraph(graph, threadNumber, profiler)
    );
}




pybind11::object Python_LoadAndRunGraph(
        pybind11::tuple args,
        pybind11::dict kwargs
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

//    pybind11::def()

    registered = true;
}
