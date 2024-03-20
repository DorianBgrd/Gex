#include "python/include/Graph.h"
#include "Gex/include/Node.h"
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


boost::python::object Gex_Graph_FindNode(boost::python::tuple args,
                                         boost::python::dict kwargs)
{
    Gex::Graph* graph = boost::python::extract<Gex::Graph*>(args[0]);

    std::string path = boost::python::extract<std::string>(args[1]);
    Gex::Node* node = graph->GetNode(path);
    if (!node)
        return {};

    return boost::python::object(boost::python::ptr(node));
}


boost::python::object Gex_Graph_FindAttribute(boost::python::tuple args,
                                              boost::python::dict kwargs)
{
    Gex::Graph* graph = boost::python::extract<Gex::Graph*>(args[0]);

    std::string path = boost::python::extract<std::string>(args[1]);
    Gex::Attribute* attr = graph->FindAttribute(path);
    if (!attr)
        return {};

    return boost::python::object(boost::python::ptr(attr));
}


bool Gex::Python::Graph_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return false;

    boost::python::class_<Gex::Graph, boost::python::bases<Gex::CompoundNode>,
            boost::noncopyable>("Graph", boost::python::init())
            .def("FindNode", boost::python::raw_function(Gex_Graph_FindNode, 1))
            .def("FindAttribute", boost::python::raw_function(Gex_Graph_FindAttribute, 1))
            ;

    pythonRegistered = true;
    return true;
}