#include "python/include/PluginLoader.h"


bool Gex::Python::PluginLoader_Wrap::pythonRegistered = false;


boost::python::object Gex_PluginLoader_RegisterNode(boost::python::tuple args,
                                                    boost::python::dict kwargs)
{
    Gex::PluginLoader* loader = boost::python::extract<Gex::PluginLoader*>(args[0]);
    std::string type = boost::python::extract<std::string>(args[1]);
    boost::python::object builder = args[2];

    boost::python::object builderInst = builder();
    Py_IncRef(builderInst.ptr());

    builderInst.attr("RegisterInLoader")(type, loader);

    return {};
}


bool Gex::Python::PluginLoader_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
    {
        return false;
    }

    bool(*Load)(std::string) = &Gex::PluginLoader::LoadPlugin;

    boost::python::class_<Gex::PluginLoader>("PluginLoader", boost::python::no_init)
            .def("RegisterNode", boost::python::raw_function(&Gex_PluginLoader_RegisterNode, 2))
            .def("LoadPlugin", Load)
            .staticmethod("LoadPlugin")
            ;

    pythonRegistered= true;
    return true;
}
