#include "Gex/include/Wrappers.h"

#include "Gex/include/Status_Wrap.h"
#include "Gex/include/Attribute_Wrap.h"
#include "Gex/include/NodeAttributeData_Wrap.h"
#include "Gex/include/Graph_Wrap.h"
#include "Gex/include/Node_Wrap.h"
#include "Gex/include/PluginLoader_Wrap.h"
#include "Gex/include/NodeFactory_Wrap.h"
#include "Gex/include/References_Wrap.h"
#include "Gex/include/io_Wrap.h"
#include "Gex/include/UndoStack_Wrap.h"
#include "Gex/include/Profiler_Wrap.h"

#include "pybind11/pybind11.h"

void Gex::Python::RegisterPythonWrappers(pybind11::module_& mod,
                                         PyThreadState* state)
{
    Gex::Python::Feedback_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::Attribute_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::GraphContext_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::Node_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::CompoundNode_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::NodeAttributeData_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::NodeBuilder_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::NodeFactory_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::References_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::IO::RegisterPythonWrapper(mod, state);

    Gex::Python::UndoStack_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::Event_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::EvaluationProfiler_Wrap::RegisterPythonWrapper(mod, state);

    Gex::Python::EvaluationNodeProfiler_Wrap::RegisterPythonWrapper(mod, state);
}


void Gex::Python::RegisterLocalModule(PyThreadState* state)
{
    std::string name = "_Gex_Runtime";

    auto sys = pybind11::module::import("sys");

    if (sys.attr("modules").contains(name.c_str()))
    {
        bool r = true;
    }

    pybind11::module_ mod = pybind11::module_::create_extension_module(
            name.c_str(), nullptr, new PyModuleDef(),
            pybind11::mod_gil_not_used(true)
    );

    RegisterPythonWrappers(mod, state);

    sys.attr("modules")[name.c_str()] = mod;
}