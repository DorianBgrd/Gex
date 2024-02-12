#include "python/include/Graph.h"
#include "include/Node.h"
#include "boost/python.hpp"


bool Gex::Python::GraphContext_Wrap::pythonRegistered = false;


boost::python::object GraphContext_Resources(boost::python::tuple args,
                                             boost::python::dict kwargs)
{
    Gex::GraphContext* context = boost::python::extract<Gex::GraphContext*>(args[0]);

    boost::python::list pythonResources;
    auto resources = context->Resources();
    for (auto r : resources)
    {
        pythonResources.append(r);
    }

    return pythonResources;
}


bool Gex::Python::GraphContext_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return false;

    boost::python::class_<Gex::GraphContext, boost::noncopyable>("GraphContext", boost::python::no_init)
            .def("RegisterResource", &Gex::GraphContext::RegisterResource)
            .def("Resources", boost::python::raw_function(&GraphContext_Resources));

    pythonRegistered  = true;
    return true;
}


bool Gex::Python::Graph_Wrap::pythonRegistered = false;


boost::python::object Gex_Graph_CreateNode(boost::python::tuple args,
                                           boost::python::dict kwargs)
{
    Gex::Graph* graph = boost::python::extract<Gex::Graph*>(args[0]);

    std::string type = boost::python::extract<std::string>(args[1]);
    std::string name = "";
    if (boost::python::len(args) > 2)
    {
        name = boost::python::extract<std::string>(args[2]);
    }

    auto* node = graph->CreateNode(type, name);
    if (!node)
    {
        return {};
    }

    return boost::python::object(boost::python::ptr(node));
}


bool Gex::Python::Graph_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return false;

    bool(Gex::Graph::*RemoveNodeStr)(std::string) = &Gex::Graph::RemoveNode;
    bool(Gex::Graph::*RemoveNodePtr)(Gex::Node*) = &Gex::Graph::RemoveNode;

    boost::python::class_<Gex::Graph, boost::noncopyable>("Graph", boost::python::init())
            .def("CreateNode", boost::python::raw_function(&Gex_Graph_CreateNode))
            .def("RemoveNode", RemoveNodeStr)
            .def("RemoveNode", RemoveNodePtr)
            .def("FindNode", &Gex::Graph::FindNode,
                 boost::python::return_internal_reference())
            ;

    pythonRegistered = true;
    return true;
}