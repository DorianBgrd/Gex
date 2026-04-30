#include "Gex/include/Gex/Wrappers.h"

#include "Gex/include/Gex/Status_Wrap.h"
#include "Gex/include/Gex/Attribute_Wrap.h"
#include "Gex/include/Gex/NodeAttributeData_Wrap.h"
#include "Gex/include/Gex/Graph_Wrap.h"
#include "Gex/include/Gex/Node_Wrap.h"
#include "Gex/include/Gex/PluginLoader_Wrap.h"
#include "Gex/include/Gex/NodeFactory_Wrap.h"
#include "Gex/include/Gex/References_Wrap.h"
#include "Gex/include/Gex/io_Wrap.h"
#include "Gex/include/Gex/UndoStack_Wrap.h"
#include "Gex/include/Gex/Profiler_Wrap.h"

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