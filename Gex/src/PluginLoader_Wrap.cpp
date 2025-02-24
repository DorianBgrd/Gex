#include "Gex/include/PluginLoader_Wrap.h"
#include "Gex/include/NodeFactory_Wrap.h"


bool Gex::Python::PluginLoader_Wrap::pythonRegistered = false;


bool Gex_PluginLoader_LoadPlugin(boost::python::tuple args,
                                 boost::python::dict kwargs)
{
    const std::string name = boost::python::extract<const std::string>(args[0]);
//    const Gex::PluginType type = boost::python::extract<const Gex::PluginType>(args[1]);

    Gex::Feedback* res = nullptr;
    if (boost::python::len(args) > 1)
        res = boost::python::extract<Gex::Feedback*>(args[1]);

    return Gex::PluginLoader::LoadPlugin(name, res);
}


boost::python::object Gex_PluginLoader_RegisterNode(boost::python::tuple args,
                                                    boost::python::dict kwargs)
{
    Gex::PluginLoader* loader = boost::python::extract<Gex::PluginLoader*>(args[0]);
    std::string type = boost::python::extract<std::string>(args[1]);
    boost::python::object builderType = args[2];

    boost::python::object builderInst = builderType();
    Py_IncRef(builderInst.ptr());

    Gex::Python::NodeBuilder_Wrap* builder = boost::python::extract<Gex::Python::NodeBuilder_Wrap*>(builderInst);

    loader->RegisterNode(type, builder);
    return {};
}


bool Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
    {
        return false;
    }

    boost::python::enum_<Gex::PluginType>("PluginType")
            .value("Library", Gex::PluginType::Library)
            .value("Python", Gex::PluginType::Python)
            ;

    boost::python::class_<Gex::PluginLoader>("PluginLoader", boost::python::no_init)
            .def("RegisterNode", boost::python::raw_function(&Gex_PluginLoader_RegisterNode, 2))
            .def("LoadPlugin", boost::python::raw_function(Gex_PluginLoader_LoadPlugin, 2))
            .staticmethod("LoadPlugin")
            ;

    pythonRegistered= true;
    return true;
}
