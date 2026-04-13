#include "Gex/include/PluginLoader_Wrap.h"
#include "Gex/include/NodeFactory_Wrap.h"


bool Gex::Python::PluginLoader_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::PluginLoader_Wrap::registry;


bool Gex_PluginLoader_LoadPlugin(pybind11::args args,
                                 pybind11::kwargs kwargs)
{
    const std::string name = args[0].cast<const std::string>();
//    const Gex::PluginType type = pybind11::extract<const Gex::PluginType>(args[1]);

    Gex::Feedback* res = nullptr;
    if (pybind11::len(args) > 1)
        res = args[1].cast<Gex::Feedback*>();

    return Gex::PluginLoader::LoadPlugin(name, res);
}


pybind11::object Gex_PluginLoader_RegisterNode(pybind11::args args,
                                               pybind11::kwargs kwargs)
{
    Gex::PluginLoader* loader = args[0].cast<Gex::PluginLoader*>();
    std::string type = args[1].cast<std::string>();
    pybind11::object builderType = args[2];

    pybind11::object builderInst = builderType();
    Py_IncRef(builderInst.ptr());

    Gex::Python::NodeBuilder_Wrap* builder = builderInst.cast<Gex::Python::NodeBuilder_Wrap*>();

    loader->RegisterNode(type, builder);
    return {};
}


bool Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                           PyThreadState* state)
{
    if (registry.IsRegistered(state))
    {
        return false;
    }

    pybind11::enum_<Gex::PluginType>(mod, "PluginType", pybind11::module_local(false))
            .value("Library", Gex::PluginType::Library)
            .value("Python", Gex::PluginType::Python)
            ;

    pybind11::class_<Gex::PluginLoader>(mod, "PluginLoader", pybind11::module_local(false))
            .def("RegisterNode", &Gex_PluginLoader_RegisterNode)
            .def_static("LoadPlugin", &Gex_PluginLoader_LoadPlugin)
            ;

    return registry.Register(state);
}


bool Gex::Python::PluginLoader_Wrap::IsRegistered()
{
    return pythonRegistered;
}
