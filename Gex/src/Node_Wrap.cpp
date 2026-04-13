#include "Gex/include/Node.h"
#include "Gex/include/Node_Wrap.h"
#include "Gex/include/Attribute.h"

#include "Tsys/tsys.h"

#include "Gex/include/wraputils.h"


std::any InitPythonValue(pybind11::object pythonType, Gex::Feedback& feedback)
{
    auto handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(pythonType);
    if (!handler)
    {
        feedback.status = Gex::Status::Failed;
        feedback.message = "No type handler found for passed python type object.";
        return {};
    }

    std::any value;

    std::string pyTypeName = pythonType
            .attr("__class__")
            .attr("__name__")
            .cast<std::string>();

    if (pyTypeName == "type")  // An object type has been passed, init a default value.
        value = handler->InitValue();
    else
        value = handler->FromPython(pythonType);

    feedback.status = Gex::Status::Success;
    return value;
}


pybind11::object Gex_Node_CreateAttribute(pybind11::tuple args,
                                               pybind11::dict kwargs)
{
    Gex::Node* self = args[0].cast<Gex::Node*>();
    std::string name = args[1].cast<std::string>();
    pybind11::object type = args[2];
    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
    Gex::AttrType attrType = Gex::AttrType::Input;
    Gex::AttributeWkPtr parent;

    if (pybind11::len(args) > 3)
    {
        attrValueType = args[3].cast<Gex::AttrValueType>();
    }
    else if (kwargs.contains("valueType"))
    {
        attrValueType = kwargs["valueType"].cast<Gex::AttrValueType>();
    }

    if (pybind11::len(args) > 4)
    {
        attrType = args[4].cast<Gex::AttrType>();
    }
    else if (kwargs.contains("type"))
    {
        attrType = kwargs["type"].cast<Gex::AttrType>();
    }

    pybind11::object pt;
    if (pybind11::len(args) > 5)
    {
        parent = args[5].cast<Gex::AttributeWkPtr>();
    }
    else if (kwargs.contains("parent"))
    {
        parent = kwargs["parent"].cast<Gex::AttributeWkPtr>();
    }

    Gex::Feedback feedback;
    std::any value = InitPythonValue(type, feedback);

    Gex::AttributeWkPtr attribute = self->CreateAttributeFromValue(
            name, value, attrValueType, attrType, parent);
    if (!attribute)
    {
        return {};
    }

    return pybind11::cast(attribute);
}



//pybind11::object Gex_CompoundNode_CreateInternalAttribute(pybind11::tuple args,
//                                                               pybind11::dict kwargs)
//{
//    Gex::CompoundNode* self = pybind11::extract<Gex::CompoundNode*>(args[0]);
//    std::string name = pybind11::extract<std::string>(args[1]);
//    pybind11::object type = args[2];
//    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
//    Gex::AttrType attrType = Gex::AttrType::Input;
//    Gex::Attribute* parent = nullptr;
//
//    if (pybind11::len(args) > 3)
//    {
//        attrValueType = pybind11::extract<Gex::AttrValueType>(args[3]);
//    }
//    else if (pybind11::object vt = kwargs.get("valueType"))
//    {
//        attrValueType = pybind11::extract<Gex::AttrValueType>(vt);
//    }
//
//    if (pybind11::len(args) > 4)
//    {
//        attrType = pybind11::extract<Gex::AttrType>(args[4]);
//    }
//    else if (pybind11::object vty = kwargs.get("type"))
//    {
//        attrType = pybind11::extract<Gex::AttrType>(vty);
//    }
//
//    if (pybind11::len(args) > 5)
//    {
//        parent = pybind11::extract<Gex::Attribute*>(args[5]);
//    }
//    else if (pybind11::object pt = kwargs.get("parent"))
//    {
//        parent = pybind11::extract<Gex::Attribute*>(pt);
//    }
//
//    Gex::Feedback feedback;
//    std::any value = InitPythonValue(type, feedback);
//
//    Gex::Attribute* attribute = self->CreateInternalAttribute(name, value, attrValueType,
//                                                              attrType, parent);
//    if (!attribute)
//    {
//        return {};
//    }
//
//    return pybind11::object(attribute);
//}
//



bool Gex::Python::Node_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::Node_Wrap::registry;


bool Gex::Python::CompoundNode_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::CompoundNode_Wrap::registry;


Gex::Python::Node_Wrap::Node_Wrap(): Gex::Node()
{

}


Gex::Python::Node_Wrap::~Node_Wrap()
{

}


std::string Gex::Python::Node_Wrap::Type() const
{
    PYBIND11_OVERRIDE(
            std::string,
            Gex::Node,
            Type
    );
}


std::string Gex::Python::Node_Wrap::Description() const
{
    PYBIND11_OVERRIDE(
            std::string,
            Gex::Node,
            Description
    );
}


void Gex::Python::Node_Wrap::InitAttributes()
{
    PYBIND11_OVERRIDE(
            void,
            Gex::Node,
            InitAttributes
    );
}


bool Gex::Python::Node_Wrap::Evaluate(NodeAttributeData &evalCtx,
                                      GraphContext &graphCtx,
                                      NodeProfiler& profiler)
{
    PYBIND11_OVERRIDE(
            bool,
            Gex::Node,
            Evaluate,
            evalCtx,
            graphCtx,
            profiler
    );
}


void Gex::Python::Node_Wrap::AttributeChanged(const Gex::AttributePtr& attribute,
                                              const Gex::AttributeChange& change)
{
    PYBIND11_OVERRIDE(
            void,
            Gex::Node,
            AttributeChanged,
            attribute,
            change
    );
}


pybind11::object NW_Python_CreateAttribute(pybind11::args args,
                                           pybind11::kwargs kwargs)
{
    Gex::Node* self = args[0].cast<Gex::Node*>();
    std::string name = args[1].cast<std::string>();
    pybind11::object type = args[2];
    Gex::AttrType attrType = Gex::AttrType::Static;
    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
    Gex::AttributePtr parent = nullptr;

    int length = pybind11::len(args);
    if (length > 3)
        attrType = args[3].cast<Gex::AttrType>();
    else if (kwargs.contains("type"))
        attrType = kwargs["type"].cast<Gex::AttrType>();

    if (length > 4)
        attrValueType = args[4].cast<Gex::AttrValueType>();
    else if (kwargs.contains("valueType"))
        attrValueType = kwargs["valueType"].cast<Gex::AttrValueType>();


    if (length > 5)
        parent = args[5].cast<Gex::AttributePtr>();
    else if (kwargs.contains("parent"))
        parent = kwargs["parent"].cast<Gex::AttributePtr>();

    Gex::Feedback feedback;
    std::any value = InitPythonValue(type, feedback);
    if (!feedback)
    {
        return {};
    }

    return pybind11::cast(self->CreateAttributeFromValue(
            name, value, attrValueType, attrType, parent));
}


pybind11::object NW_Python_CreateAttributeFromTypeName(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::Node* self = args[0].cast<Gex::Node*>();
    std::string name = args[1].cast<std::string>();
    pybind11::object type = args[2];
    Gex::AttrType attrType = Gex::AttrType::Static;
    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
    Gex::AttributePtr parent = nullptr;

    int length = pybind11::len(args);
    if (length > 3)
        attrType = args[3].cast<Gex::AttrType>();
    else if (kwargs.contains("type"))
        attrType = kwargs["type"].cast<Gex::AttrType>();

    if (length > 4)
        attrValueType = args[4].cast<Gex::AttrValueType>();
    else if (kwargs.contains("valueType"))
        attrValueType = kwargs["valueType"].cast<Gex::AttrValueType>();

    if (length > 5)
        parent = args[5].cast<Gex::AttributePtr>();
    else if (kwargs.contains("parent"))
        parent = kwargs["parent"].cast<Gex::AttributePtr>();

//    std::any value = InitPythonValue(type, feedback);
    std::string apiType = type.cast<std::string>();

    return pybind11::cast(self->CreateAttributeFromTypeName(
            name, apiType, attrValueType, attrType, parent));
}


template<class T>
inline pybind11::list PtrVectorToBoostPtrList(std::vector<T*> vec)
{
    pybind11::list l;
    for (T* value : vec)
    {
        l.append(pybind11::object(value));
    }

    return l;
}


template<class T>
inline pybind11::list VectorToBoostList(std::vector<T> vec)
{
    pybind11::list l;
    for (T value : vec)
    {
        l.append(value);
    }

    return l;
}


pybind11::object NW_Python_GetAttributes(pybind11::args args,
                                         pybind11::kwargs kwargs)
{
    Gex::NodePtr self = args[0].cast<Gex::NodePtr>();

    return VectorToBoostList(self->GetAllAttributes());
}


pybind11::object NW_Python_UpstreamNodes(pybind11::args args,
                                         pybind11::kwargs kwargs)
{
    Gex::NodePtr self = args[0].cast<Gex::NodePtr>();

    return VectorToBoostList(self->UpstreamNodes());
}


pybind11::object MakeWeakReference(pybind11::args args,
                                   pybind11::kwargs kwargs)
{
    Gex::NodePtr node = args[0].cast<Gex::NodePtr>();

    Gex::NodeWkPtr wknode = node;

    return pybind11::cast(wknode);
}


pybind11::object Node_RegisterAttributeCallback(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    Gex::Node* self = args[0].cast<Gex::Node*>();

    pybind11::object callable = args[1];

    auto id = self->RegisterAttributeCallback(
            [callable](const Gex::AttributePtr& attribute,
                       const Gex::AttributeChange& change)
            {
                callable(pybind11::cast(attribute),
                         pybind11::cast(change));
            }
    );

    return pybind11::cast(id);
}


pybind11::object Node_RegisterNodeChangedCallback(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    Gex::Node* self = args[0].cast<Gex::Node*>();

    pybind11::object callable = args[1];

    auto id = self->RegisterNodeChangedCallback(
            [callable](const Gex::NodeChange& c, const Gex::NodeWkPtr& n)
            {
                callable(pybind11::cast(c),
                         pybind11::cast(n));
            }
    );

    return pybind11::cast(id);
}


pybind11::object Node_Bool(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::Node* self = args[0].cast<Gex::Node*>();

    return pybind11::cast(bool(self));
}


bool Gex::Python::Node_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                   PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    /*
     * ChildNodeAdded,
        ChildNodeRemoved,
        AttributeAdded,
        AttributeRemoved,
        Deleted
     */

    pybind11::enum_<Gex::NodeChange>(mod, "NodeChange", pybind11::module_local(false))
            .value("ChildNodeAdded", Gex::NodeChange::ChildNodeAdded)
            .value("ChildNodeRemoved", Gex::NodeChange::ChildNodeRemoved)
            .value("AttributeAdded", Gex::NodeChange::AttributeAdded)
            .value("AttributeRemoved", Gex::NodeChange::AttributeRemoved)
            .value("Deleted", Gex::NodeChange::Deleted)
            ;


    pybind11::class_<Gex::Node, Gex::Python::Node_Wrap, Gex::NodePtr>(mod, "Node", pybind11::module_local(false))
            .def(pybind11::init<>())
            .def("InitAttributes", &Gex::Node::InitAttributes)
            .def("Evaluate", &Gex::Node::Evaluate)
            .def("Name", &Gex::Node::Name)
            .def("Type", &Gex::Node::Type)
            .def("Description", &Gex::Node::Description)
//            .def("CreateAttributeFromValue", pybind11::raw_function(&NW_Python_CreateAttribute, 3))
            .def("CreateAttributeFromTypeName", &NW_Python_CreateAttributeFromTypeName)
            .def("GetAttributes", &NW_Python_GetAttributes)
            .def("GetAttribute", &Gex::Node::GetAttribute)
            .def("IsEditable", &Gex::Node::IsEditable)
            .def("SetEditable", &Gex::Node::SetEditable)
            .def("Compute", &Gex::Node::Compute)
            .def("UpstreamNodes", &NW_Python_UpstreamNodes)
            .def("HasAttribute", &Gex::Node::HasAttribute)
            .def("AttributeChanged", &Gex::Node::AttributeChanged)
            .def("RegisterAttributeChangedCallback", &Node_RegisterAttributeCallback)
            .def("RegisterNodeChangedCallback", &Node_RegisterNodeChangedCallback)
            .def("__bool__", &Node_Bool)
            ;

//    pybind11::class_<Gex::NodeWkPtr>("NodeWk", pybind11::no_init)
//            .def("IsWkValid", &Gex::NodeWkPtr::expired)
//            .def("ToNode", &Gex::NodeWkPtr::lock)
//            .def("__call__", &Gex::NodeWkPtr::lock)
//            .def("__bool__", &Gex::NodeWkPtr::operator bool)
//            ;

    return registry.Register(state);
}


bool Gex::Python::Node_Wrap::IsRegistered()
{
    return pythonRegistered;
}


pybind11::object CN_Python_GetInternalNode(pybind11::args args, pybind11::kwargs kwargs)
{
    const Gex::Python::CompoundNode_Wrap& self = args[0].cast<const Gex::Python::CompoundNode_Wrap&>();
    std::string name = args[1].cast<std::string>();

    return pybind11::cast(self.GetNode(name));
}


pybind11::object CN_Python_FromNode(pybind11::args args, pybind11::kwargs kwargs)
{
    Gex::NodePtr self = args[0].cast<Gex::NodePtr>();

    return pybind11::cast(Gex::CompoundNode::FromNode(self));
}


pybind11::object CN_Python_GetNodes(pybind11::args args, pybind11::kwargs kwargs)
{
    Gex::CompoundNode* self = args[0].cast<Gex::CompoundNode*>();

    return VectorToBoostList(self->GetNodes());
}


pybind11::object CN_Python_GetNodeNames(pybind11::args args, pybind11::kwargs kwargs)
{
    Gex::CompoundNode* self = args[0].cast<Gex::CompoundNode*>();

    return VectorToBoostList(self->GetNodeNames());
}


Gex::Python::CompoundNode_Wrap::CompoundNode_Wrap(): Gex::CompoundNode()
{

}


Gex::Python::CompoundNode_Wrap::~CompoundNode_Wrap()
{

}


bool Gex::Python::CompoundNode_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                           PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    bool (CompoundNode_Wrap::*_RemoveInternalNode)(const std::string&) =
    &Gex::Python::CompoundNode_Wrap::RemoveNode;
//    bool (CompoundNode_Wrap::*_HasNode)(const std::string&) =
//    &Gex::Python::CompoundNode_Wrap::HasNode;

    pybind11::class_<Gex::CompoundNode, Gex::Python::CompoundNode_Wrap, Gex::Node,
            Gex::CompoundNodePtr>(mod, "CompoundNode", pybind11::module_local(false))
            .def(pybind11::init<>())
            .def("CreateNode", &Gex::CompoundNode::CreateNode)
//            .def("GetNode", pybind11::raw_function(&CN_Python_GetInternalNode, 1))
            .def("GetNode", &Gex::CompoundNode::GetNode)
            .def("GetNodes", &CN_Python_GetNodes)
            .def("GetNodeNames", &CN_Python_GetNodeNames)
//            .def("HasNode", _HasNode)
//            .def("RemoveNode", _RemoveInternalNode)
//            .def("CreateInternalAttribute", pybind11::raw_function(&Gex_CompoundNode_CreateInternalAttribute, 3))
            .def_static("FromNode", &CN_Python_FromNode)
            ;

    return registry.Register(state);
}


bool Gex::Python::CompoundNode_Wrap::IsRegistered()
{
    return registered;
}
