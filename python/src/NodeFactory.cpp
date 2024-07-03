#include "python/include/NodeFactory.h"
#include "python/include/Node.h"



Gex::Python::NodeBuilder_Wrap::NodeBuilder_Wrap():
        Gex::DefaultNodeBuilder(),
        boost::python::wrapper<Gex::DefaultNodeBuilder>()
{

}


Gex::Python::NodeBuilder_Wrap::NodeBuilder_Wrap(const DefaultNodeBuilder base):
        Gex::DefaultNodeBuilder(base), boost::python::wrapper<Gex::DefaultNodeBuilder>()
{

}


Gex::Node* Gex::Python::NodeBuilder_Wrap::CreateNode() const
{
    if (boost::python::override o = boost::python::wrapper<DefaultNodeBuilder>::get_override("CreateNode"))
    {
        boost::python::object pythonNode = o();
        Py_IncRef(pythonNode.ptr());

        Gex::Python::Node_Wrap::RegisterPythonWrapper();

        return boost::python::extract<Gex::Node*>(pythonNode);
    }

    return Gex::DefaultNodeBuilder::CreateNode();
}


bool Gex::Python::NodeFactory_Wrap::pythonRegistered = false;


boost::python::object NF_CreateNode(boost::python::tuple args,
                                    boost::python::dict kwargs)
{
    std::string type = boost::python::extract<std::string>(args[0]);
    std::string name = boost::python::extract<std::string>(args[1]);

    auto* node = Gex::NodeFactory::GetFactory()->CreateNode(type, name);
    if (!node)
    {
        return {};
    }

    return boost::python::object(boost::python::ptr(node));
}


boost::python::object NF_TypeExists(boost::python::tuple args,
                                    boost::python::dict kwargs)
{
    std::string type = boost::python::extract<std::string>(args[0]);

    bool ex = Gex::NodeFactory::GetFactory()->TypeExists(type);
    return boost::python::object(ex);
}


boost::python::object NF_NodeTypes(boost::python::tuple args,
                                   boost::python::dict kwargs)
{
    boost::python::list types;
    for (auto t : Gex::NodeFactory::GetFactory()->NodeTypes())
    {
        types.append(t);
    }

    return types;
}


boost::python::object NF_RegisterNodeBuilder(boost::python::object args,
                                             boost::python::dict kwargs)
{
    Gex::PluginLoader* loader = boost::python::extract<Gex::PluginLoader*>(args[0]);
    std::string nodeName = boost::python::extract<std::string>(args[1]);
    boost::python::object builderType = args[2];

    boost::python::object builder = builderType();

    Gex::NodeBuilder* cppbuilder = boost::python::extract<Gex::Python::NodeBuilder_Wrap*>(builder);

    loader->RegisterNode(nodeName, cppbuilder);
}


bool Gex::Python::NodeFactory_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
    {
        return false;
    }

    boost::python::def("CreateNode", boost::python::raw_function(NF_CreateNode, 2));
    boost::python::def("TypeExists", boost::python::raw_function(NF_TypeExists, 1));
    boost::python::def("NodeTypes", boost::python::raw_function(NF_NodeTypes, 0));

    boost::python::class_<Gex::NodeFactory>("NodeFactory", boost::python::no_init)
            .def("RegisterNodeBuilder", boost::python::raw_function(NF_RegisterNodeBuilder, 3))
            ;

    boost::python::class_<Gex::Python::NodeBuilder_Wrap>("NodeBuilder", boost::python::init())
            .def("CreateNode", &Gex::Python::NodeBuilder_Wrap::CreateNode,
                 boost::python::return_internal_reference())
            ;

    pythonRegistered= true;
    return true;
}