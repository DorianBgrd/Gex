#include "Gex/include/Node.h"
#include "python/include/Node.h"
#include "Gex/include/Attribute.h"
#include "Gex/include/Graph.h"

#include "Tsys/include/tsys.h"


std::any InitPythonValue(boost::python::object pythonType, Gex::Feedback& feedback)
{
    boost::python::object class_ = pythonType.attr("__class__");

    std::string python_typename = boost::python::extract<std::string>(class_.attr("__name__"));

    bool isObject = (python_typename != "type");
    if (!isObject)
    {
        python_typename = boost::python::extract<std::string>(pythonType.attr("__name__"));
    }

    bool success = true;
    size_t hash = TSys::TypeRegistry::GetRegistry()->GetHashFromPythonType(python_typename, success);
    if (!success)
    {
        feedback.status = Gex::Status::Failed;
        feedback.message = "No type handler found for type " + python_typename;
        return {};
    }

    std::any value;

    auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(hash);
    if (!handler)
    {
        feedback.status = Gex::Status::Failed;
        feedback.message = "No type handler found for type " + python_typename;
        return {};
    }

    if (isObject)
        value = handler->InitValue();
    else
        value = handler->FromPython(pythonType);

    feedback.status = Gex::Status::Success;
    return value;
}


boost::python::object Gex_Node_CreateAttribute(boost::python::tuple args,
                                               boost::python::dict kwargs)
{
    Gex::Node* self = boost::python::extract<Gex::Node*>(args[0]);
    std::string name = boost::python::extract<std::string>(args[1]);
    boost::python::object type = args[2];
    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
    Gex::AttrType attrType = Gex::AttrType::Input;
    Gex::Attribute* parent = nullptr;

    if (boost::python::len(args) > 3)
    {
        attrValueType = boost::python::extract<Gex::AttrValueType>(args[3]);
    }
    else if (boost::python::object vt = kwargs.get("valueType"))
    {
        attrValueType = boost::python::extract<Gex::AttrValueType>(vt);
    }

    if (boost::python::len(args) > 4)
    {
        attrType = boost::python::extract<Gex::AttrType>(args[4]);
    }
    else if (boost::python::object vty = kwargs.get("type"))
    {
        attrType = boost::python::extract<Gex::AttrType>(vty);
    }

    if (boost::python::len(args) > 5)
    {
        parent = boost::python::extract<Gex::Attribute*>(args[5]);
    }
    else if (boost::python::object pt = kwargs.get("parent"))
    {
        parent = boost::python::extract<Gex::Attribute*>(pt);
    }

    Gex::Feedback feedback;
    std::any value = InitPythonValue(type, feedback);

    Gex::Attribute* attribute = self->CreateAttributeFromValue(name, value, attrValueType,
                                                               attrType, parent);
    if (!attribute)
    {
        return {};
    }

    return boost::python::object(boost::python::ptr(attribute));
}



boost::python::object Gex_CompoundNode_CreateInternalAttribute(boost::python::tuple args,
                                                               boost::python::dict kwargs)
{
    Gex::CompoundNode* self = boost::python::extract<Gex::CompoundNode*>(args[0]);
    std::string name = boost::python::extract<std::string>(args[1]);
    boost::python::object type = args[2];
    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
    Gex::AttrType attrType = Gex::AttrType::Input;
    Gex::Attribute* parent = nullptr;

    if (boost::python::len(args) > 3)
    {
        attrValueType = boost::python::extract<Gex::AttrValueType>(args[3]);
    }
    else if (boost::python::object vt = kwargs.get("valueType"))
    {
        attrValueType = boost::python::extract<Gex::AttrValueType>(vt);
    }

    if (boost::python::len(args) > 4)
    {
        attrType = boost::python::extract<Gex::AttrType>(args[4]);
    }
    else if (boost::python::object vty = kwargs.get("type"))
    {
        attrType = boost::python::extract<Gex::AttrType>(vty);
    }

    if (boost::python::len(args) > 5)
    {
        parent = boost::python::extract<Gex::Attribute*>(args[5]);
    }
    else if (boost::python::object pt = kwargs.get("parent"))
    {
        parent = boost::python::extract<Gex::Attribute*>(pt);
    }

    Gex::Feedback feedback;
    std::any value = InitPythonValue(type, feedback);

    Gex::Attribute* attribute = self->CreateInternalAttribute(name, value, attrValueType,
                                                              attrType, parent);
    if (!attribute)
    {
        return {};
    }

    return boost::python::object(boost::python::ptr(attribute));
}




bool Gex::Python::Node_Wrap::pythonRegistered = false;


bool Gex::Python::CompoundNode_Wrap::registered = false;


Gex::Python::Node_Wrap::Node_Wrap(): Gex::Node(), boost::python::wrapper<Gex::Node>()
{

}


Gex::Python::Node_Wrap::Node_Wrap(const Gex::Node base) : Gex::Node(base), boost::python::wrapper<Node>()
{

};


std::string Gex::Python::Node_Wrap::Type() const
{
    if (boost::python::override func = boost::python::wrapper<Gex::Node>::get_override("Type"))
    {
        return func();
    }

    return Node::Type();
}


std::string Gex::Python::Node_Wrap::Description() const
{
    if (boost::python::override func = this->get_override("Description"))
    {
        boost::python::object r = func();
        std::string res = boost::python::extract<std::string>(r);
        return res;
    }

    return this->Node::Description();
}


void Gex::Python::Node_Wrap::InitAttributes()
{
    if (boost::python::override func = boost::python::wrapper<Gex::Node>::get_override("InitAttributes"))
    {
        func();
    }
    else
    {
        Node::InitAttributes();
    }
}


bool Gex::Python::Node_Wrap::Evaluate(NodeAttributeData &evalCtx,
                                      GraphContext &graphCtx,
                                      NodeProfiler& profiler)
{
    if (boost::python::override func = boost::python::wrapper<Gex::Node>::get_override("Evaluate"))
    {
        try
        {
            boost::python::object res = func(evalCtx, graphCtx);
            bool result = boost::python::extract<bool>(res);
            return result;
        }
        catch(const boost::python::error_already_set&)
        {
            PyErr_Print();
            return false;
        }

    }

    return Node::Evaluate(evalCtx, graphCtx, profiler);
}


void Gex::Python::Node_Wrap::AttributeChanged(Gex::Attribute* attribute, Gex::AttributeChange change)
{
    if (boost::python::override func = boost::python::wrapper<Gex::Node>::get_override("AttributeChanged"))
    {
        func(attribute, change);
    }
    else
    {
        Node::AttributeChanged(attribute, change);
    }
}


boost::python::object NW_Python_CreateAttribute(boost::python::tuple args,
                                                boost::python::dict kwargs)
{
    Gex::Node* self = boost::python::extract<Gex::Node*>(args[0]);
    std::string name = boost::python::extract<std::string>(args[1]);
    boost::python::object type = args[2];
    Gex::AttrType attrType = Gex::AttrType::Static;
    Gex::AttrValueType attrValueType = Gex::AttrValueType::Single;
    Gex::Attribute* parent = nullptr;

    int length = boost::python::len(args);
    if (length > 3)
        attrType = boost::python::extract<Gex::AttrType>(args[3]);
    else if (kwargs.has_key("type"))
        attrType = boost::python::extract<Gex::AttrType>(kwargs["type"]);

    if (length > 4)
        attrValueType = boost::python::extract<Gex::AttrValueType>(args[4]);
    else if (kwargs.has_key("valueType"))
        attrValueType = boost::python::extract<Gex::AttrValueType>(kwargs["valueType"]);


    if (length > 5)
        parent = boost::python::extract<Gex::Attribute*>(args[5]);
    else if (kwargs.has_key("parent"))
        parent = boost::python::extract<Gex::Attribute*>(kwargs["parent"]);

    Gex::Feedback feedback;
    std::any value = InitPythonValue(type, feedback);
    if (!feedback)
    {
        return {};
    }

    return boost::python::object(boost::python::ptr(self->CreateAttributeFromValue(name, value, attrValueType, attrType, parent)));
}


template<class T>
inline boost::python::list PtrVectorToBoostPtrList(std::vector<T*> vec)
{
    boost::python::list l;
    for (T* value : vec)
    {
        l.append(boost::python::object(boost::python::ptr(value)));
    }

    return l;
}


boost::python::object NW_Python_GetAttributes(boost::python::tuple args,
                                              boost::python::dict kwargs)
{
    Gex::Node* self = boost::python::extract<Gex::Node*>(args[0]);

    return PtrVectorToBoostPtrList(self->GetAllAttributes());
}


boost::python::object NW_Python_UpstreamNodes(boost::python::tuple args,
                                              boost::python::dict kwargs)
{
    Gex::Node* self = boost::python::extract<Gex::Node*>(args[0]);

    return PtrVectorToBoostPtrList(self->UpstreamNodes());
}


void Gex::Python::Node_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return;

    boost::python::class_<Gex::Python::Node_Wrap, boost::noncopyable>("Node", boost::python::init())
            .def("InitAttributes", &Gex::Node::InitAttributes)
            .def("Evaluate", &Gex::Node::Evaluate)
            .def("Name", &Gex::Node::Name)
            .def("Type", &Gex::Node::Type)
            .def("Description", &Gex::Node::Description)
            .def("ComputeHash", &Gex::Node::ComputeHash)
            .def("CreateAttributeFromValue", boost::python::raw_function(&NW_Python_CreateAttribute, 3))
            .def("GetAttributes", boost::python::raw_function(&NW_Python_GetAttributes, 1))
            .def("GetAttribute", &Gex::Node::GetAttribute,
                 boost::python::return_internal_reference())
            .def("IsEditable", &Gex::Node::IsEditable)
            .def("IsDirty", &Gex::Node::IsDirty)
            .def("SetEditable", &Gex::Node::SetEditable)
            .def("Compute", &Gex::Node::Compute)
            .def("UpstreamNodes", boost::python::raw_function(&NW_Python_UpstreamNodes, 1))
            .def("HasAttribute", &Gex::Node::HasAttribute)
            .def("AttributeChanged", &Gex::Node::AttributeChanged)
            ;

    pythonRegistered = true;
}



boost::python::object CN_Python_GetInternalNode(boost::python::tuple args, boost::python::dict kwargs)
{
    Gex::Python::CompoundNode_Wrap self = boost::python::extract<Gex::Python::CompoundNode_Wrap>(args[0]);
    std::string name = boost::python::extract<std::string>(args[1]);

    return boost::python::object(boost::python::ptr(self.GetInternalNode(name)));
}


boost::python::object CN_Python_FromNode(boost::python::tuple args, boost::python::dict kwargs)
{
    Gex::Node* self = boost::python::extract<Gex::Node*>(args[0]);

    return boost::python::object(boost::python::ptr(Gex::CompoundNode::FromNode(self)));
}


Gex::Python::CompoundNode_Wrap::CompoundNode_Wrap(): Gex::CompoundNode(),
     boost::python::wrapper<Gex::CompoundNode>()
{

}


Gex::Python::CompoundNode_Wrap::CompoundNode_Wrap(const Gex::CompoundNode& base) : Gex::CompoundNode(base),
     boost::python::wrapper<Gex::CompoundNode>()
{

};


void Gex::Python::CompoundNode_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return;

    bool (CompoundNode_Wrap::*_RemoveInternalNode)(const std::string&) =
    &Gex::Python::CompoundNode_Wrap::RemoveInternalNode;
    bool (CompoundNode_Wrap::*_IsInternalNode)(const std::string&) =
    &Gex::Python::CompoundNode_Wrap::RemoveInternalNode;

    boost::python::class_<Gex::Python::CompoundNode_Wrap, boost::python::bases<Gex::Node>,
            boost::noncopyable>("CompoundNode", boost::python::init())
            .def("CreateInternalNode", &Gex::CompoundNode::CreateInternalNode,
                 boost::python::return_internal_reference())
            .def("GetInternalNode", boost::python::raw_function(&CN_Python_GetInternalNode, 1))
            .def("IsInternalNode", _IsInternalNode)
            .def("RemoveInternalNode", _RemoveInternalNode)
            .def("CreateInternalAttribute", boost::python::raw_function(&Gex_CompoundNode_CreateInternalAttribute, 3))
            .def("FromNode", boost::python::raw_function(&CN_Python_FromNode, 1))
            .staticmethod("FromNode")
            ;

    registered = true;
}
