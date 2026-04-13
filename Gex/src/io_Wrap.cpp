#include "Gex/include/io_Wrap.h"
#include "Gex/include/io.h"


bool Gex::Python::IO::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::IO::registry;



pybind11::object Python_SaveGraph(pybind11::args args,
                                  pybind11::kwargs kwargs)
{
    Gex::CompoundNodePtr graph = args[0].cast<Gex::CompoundNodePtr>();
    std::string filepath = args[1].cast<std::string>();

    return pybind11::cast(Gex::Feedback(
            Gex::SaveGraph(graph, filepath)));
}


pybind11::object Python_LoadGraph(pybind11::args args,
                                  pybind11::kwargs kwargs)
{
    std::string filepath = args[0].cast<std::string>();
    pybind11::object pyfeedback;

    auto* feedback = new Gex::Feedback();
    if (pybind11::len(args) > 1)
    {
        pyfeedback = args[1];
    }
    else if (kwargs.contains("feedback"))
    {
        pyfeedback = kwargs["feedback"];
    }

    if (pyfeedback)
        feedback = pyfeedback.cast<Gex::Feedback*>();

    Gex::NodePtr graph = Gex::LoadGraph(filepath, feedback);
    if (!graph)
        return {};


    return pybind11::cast(Gex::CompoundNode::FromNode(graph));
}


pybind11::object Python_ReloadNode(
        pybind11::args args,
        pybind11::kwargs kwargs)
{
    Gex::NodePtr node = args[0].cast<Gex::NodePtr>();

    return pybind11::cast(Gex::ReloadNode(node));
}


//pybind11::object Python_ListAvailableReferences(
//        pybind11::tuple args,
//        pybind11::dict kwargs)
//{
//    pybind11::list refs;
//
//    for (const auto& r : Gex::ListAvailableReferences())
//    {
//        refs.append(r);
//    }
//
//    return refs;
//}


bool Gex::Python::IO::RegisterPythonWrapper(pybind11::module_& mod,
                                            PyThreadState* state)
{
    if (registry.IsRegistered(state))
    {
        return false;
    }

    mod.def("SaveGraph", &Python_SaveGraph);
    mod.def("LoadGraph", &Python_LoadGraph);
    mod.def("ReloadNode", &Python_ReloadNode);
//    pybind11::def("ListAvailableReferences", pybind11::raw_function(&Python_ListAvailableReferences, 0));

    return registry.Register(state);
}


bool Gex::Python::IO::IsRegistered()
{
    return pythonRegistered;
}
