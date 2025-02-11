#include "Gex/include/Attribute_Wrap.h"
#include "Gex/include/Graph_Wrap.h"
#include "Gex/include/Status_Wrap.h"
#include "Gex/include/Node_Wrap.h"
#include "Gex/include/io_Wrap.h"
#include "Gex/include/PluginLoader_Wrap.h"
#include "Gex/include/NodeFactory_Wrap.h"
#include "Gex/include/References_Wrap.h"
#include "Gex/include/UndoStack_Wrap.h"
#include "Gex/include/Profiler_Wrap.h"

#include "boost/python.hpp"


#include "Tsys/include/tsys.h"


boost::python::object GetAvailableTypes(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    boost::python::list types;
    for (const std::string& t : TSys::TypeRegistry::GetRegistry()->RegisteredTypes())
    {
        types.append(boost::python::object(t));
    }

    return types;
}


BOOST_PYTHON_MODULE(Gex_Python)
{
    boost::python::def("GetAvailableTypes", boost::python::raw_function(GetAvailableTypes));

    Gex::Python::Feedback_Wrap::RegisterPythonWrapper();

    Gex::Python::Attribute_Wrap::RegisterPythonWrapper();

    Gex::Python::GraphContext_Wrap::RegisterPythonWrapper();

    Gex::Python::Node_Wrap::RegisterPythonWrapper();

    Gex::Python::CompoundNode_Wrap::RegisterPythonWrapper();

    Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper();

    Gex::Python::NodeBuilder_Wrap::RegisterPythonWrapper();

    Gex::Python::NodeFactory_Wrap::RegisterPythonWrapper();

    Gex::Python::References_Wrap::RegisterPythonWrapper();

    Gex::Python::IO::RegisterPythonWrapper();

    Gex::Python::UndoStack_Wrap::RegisterPythonWrapper();

    Gex::Python::Event_Wrap::RegisterPythonWrapper();

    Gex::Python::EvaluationProfiler_Wrap::RegisterPythonWrapper();

    Gex::Python::EvaluationNodeProfiler_Wrap::RegisterPythonWrapper();
}