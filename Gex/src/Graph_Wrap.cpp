#include "Gex/include/Gex/Graph_Wrap.h"
#include "Gex/include/Gex/Node.h"


bool Gex::Python::GraphContext_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::GraphContext_Wrap::registry;



pybind11::object GraphContext_Resources(pybind11::args args,
                                        pybind11::kwargs kwargs)
{
    Gex::GraphContext* context = args[0].cast<Gex::GraphContext*>();

    pybind11::list pythonResources;
    auto resources = context->Resources();
    for (auto r : resources)
    {
        pythonResources.append(r);
    }

    return pythonResources;
}


bool Gex::Python::GraphContext_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                           PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Gex::GraphContext>(mod, "GraphContext", pybind11::module_local(false))
            .def("RegisterResource", &Gex::GraphContext::RegisterResource)
            .def("Resources", &GraphContext_Resources);

    return registry.Register(state);
}


bool Gex::Python::GraphContext_Wrap::IsRegistered()
{
    return pythonRegistered;
}


//bool Gex::Python::Graph_Wrap::pythonRegistered = false;
//
//
//boost::python::object Gex_Graph_FindNode(boost::python::tuple args,
//                                         boost::python::dict kwargs)
//{
//    Gex::Graph* graph = boost::python::extract<Gex::Graph*>(args[0]);
//
//    std::string path = boost::python::extract<std::string>(args[1]);
//    Gex::Node* node = graph->GetNode(path);
//    if (!node)
//        return {};
//
//    return boost::python::object(boost::python::ptr(node));
//}
//
//
//boost::python::object Gex_Graph_FindAttribute(boost::python::tuple args,
//                                              boost::python::dict kwargs)
//{
//    Gex::Graph* graph = boost::python::extract<Gex::Graph*>(args[0]);
//
//    std::string path = boost::python::extract<std::string>(args[1]);
//    Gex::Attribute* attr = graph->FindAttribute(path);
//    if (!attr)
//        return {};
//
//    return boost::python::object(boost::python::ptr(attr));
//}
//
//
//bool Gex::Python::Graph_Wrap::RegisterPythonWrapper()
//{
//    if (pythonRegistered)
//        return false;
//
//    boost::python::class_<Gex::Graph, boost::python::bases<Gex::CompoundNode>,
//            boost::noncopyable>("Graph", boost::python::init())
//            .def("FindNode", boost::python::raw_function(Gex_Graph_FindNode, 1))
//            .def("FindAttribute", boost::python::raw_function(Gex_Graph_FindAttribute, 1))
//            ;
//
//    pythonRegistered = true;
//    return true;
//}