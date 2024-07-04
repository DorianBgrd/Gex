#include "Gex/include/io_Wrap.h"
#include "Gex/include/io.h"
#include "boost/python.hpp"


bool Gex::Python::IO::pythonRegistered = false;


boost::python::object Python_SaveGraph(boost::python::tuple args,
                                       boost::python::dict kwargs)
{
    Gex::CompoundNode* graph = boost::python::extract<Gex::CompoundNode*>(args[0]);
    std::string filepath = boost::python::extract<std::string>(args[1]);

    return boost::python::object(Gex::Feedback(Gex::SaveGraph(graph, filepath)));
}


boost::python::object Python_LoadGraph(boost::python::tuple args,
                                       boost::python::dict kwargs)
{
    std::string filepath = boost::python::extract<std::string>(args[0]);
    boost::python::object pyfeedback;

    auto* feedback = new Gex::Feedback();
    if (boost::python::len(args) > 1)
    {
        pyfeedback = args[1];
    }
    else
    {
        pyfeedback = kwargs.get("feedback");
    }

    if (pyfeedback)
        feedback = boost::python::extract<Gex::Feedback*>(pyfeedback);

    Gex::Node* graph = Gex::LoadGraph(filepath, feedback);
    if (!graph)
        return {};


    return boost::python::object(boost::python::ptr(Gex::CompoundNode::FromNode(graph)));
}


bool Gex::Python::IO::RegisterPythonWrapper()
{
    if (pythonRegistered)
    {
        return false;
    }

    boost::python::def("SaveGraph", boost::python::raw_function(&Python_SaveGraph, 1));
    boost::python::def("LoadGraph", boost::python::raw_function(&Python_LoadGraph, 1));

    pythonRegistered = true;
    return true;
}
