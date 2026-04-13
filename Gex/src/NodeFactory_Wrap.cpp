#include "Gex/include/NodeFactory_Wrap.h"
#include "Gex/include/Node_Wrap.h"


bool Gex::Python::NodeBuilder_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::NodeBuilder_Wrap::registry;



Gex::Python::NodeBuilder_Wrap::NodeBuilder_Wrap():
        Gex::DefaultNodeBuilder(),
        pybind11::trampoline_self_life_support()
{

}


Gex::Python::NodeBuilder_Wrap::NodeBuilder_Wrap(const DefaultNodeBuilder base):
        Gex::DefaultNodeBuilder(base),
        pybind11::trampoline_self_life_support()
{

}


Gex::Node* Gex::Python::NodeBuilder_Wrap::CreateNode() const
{
    PYBIND11_OVERRIDE(
            Gex::Node*,
            Gex::DefaultNodeBuilder,
            CreateNode
    );
}


bool Gex::Python::NodeBuilder_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                          PyThreadState* state)
{
    if (registry.IsRegistered(state))
    {
        return false;
    }

    pybind11::class_<Gex::Python::NodeBuilder_Wrap>(mod, "NodeBuilder", pybind11::module_local(false))
            .def(pybind11::init<>())
            .def("CreateNode", &Gex::Python::NodeBuilder_Wrap::CreateNode,
                 pybind11::return_value_policy::reference_internal);

    return registry.Register(state);
}


bool Gex::Python::NodeBuilder_Wrap::IsRegistered()
{
    return pythonRegistered;
}



bool Gex::Python::NodeFactory_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::NodeFactory_Wrap::registry;



pybind11::object NF_CreateNode(pybind11::args args,
                               pybind11::kwargs kwargs)
{
    std::string type = args[0].cast<std::string>();
    std::string name = args[1].cast<std::string>();

    auto node = Gex::NodeFactory::GetFactory()->CreateNode(type, name);
    if (!node)
    {
        return {};
    }

    return pybind11::cast(node);
}


pybind11::object NF_TypeExists(pybind11::args args,
                               pybind11::kwargs kwargs)
{
    std::string type = args[0].cast<std::string>();

    bool ex = Gex::NodeFactory::GetFactory()->TypeExists(type);
    return pybind11::cast(ex);
}


pybind11::object NF_NodeTypes(pybind11::args args,
                              pybind11::kwargs kwargs)
{
    pybind11::list types;
    for (auto t : Gex::NodeFactory::GetFactory()->NodeTypes())
    {
        types.append(t);
    }

    return types;
}


pybind11::object NF_RegisterNodeBuilder(pybind11::args args,
                                        pybind11::kwargs kwargs)
{
    Gex::PluginLoader* loader = args[0].cast<Gex::PluginLoader*>();
    std::string nodeName = args[1].cast<std::string>();
    pybind11::object builderType = args[2];

    pybind11::object builder = builderType();

    Gex::NodeBuilder* cppbuilder = builder.cast<Gex::Python::NodeBuilder_Wrap*>();

    loader->RegisterNode(nodeName, cppbuilder);

    return {};
}


bool Gex::Python::NodeFactory_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                          PyThreadState* state)
{
    if (registry.IsRegistered(state))
    {
        return false;
    }

    mod.def("CreateNode", NF_CreateNode);
    mod.def("TypeExists", NF_TypeExists);
    mod.def("NodeTypes", NF_NodeTypes);

    pybind11::class_<Gex::NodeFactory>(mod, "NodeFactory", pybind11::module_local(false))
            .def("RegisterNodeBuilder", &NF_RegisterNodeBuilder)
            ;

    return registry.Register(state);
}


bool Gex::Python::NodeFactory_Wrap::IsRegistered()
{
    return pythonRegistered;
}