#include "Gex/include/Attribute_Wrap.h"
#include "Gex/include/Node.h"
#include "Tsys/include/tsys.h"

#include "Gex/include/ptrs.h"

bool Gex::Python::Attribute_Wrap::registered = false;


boost::python::object Attribute_Python_Get(boost::python::tuple args,
                                           boost::python::dict kwargs)
{
    Gex::Attribute* attribute = boost::python::extract<Gex::Attribute*>(args[0]);
    if (!attribute)
    {
        return {};
    }

    return attribute->TypeHandle()->ToPython(attribute->GetAnyValue());
}


bool Attribute_Python_Set(boost::python::tuple args,
                          boost::python::dict kwargs)
{
    Gex::Attribute* attribute = boost::python::extract<Gex::Attribute*>(args[0]);
    if (!attribute)
    {
        return false;
    }

    if (!attribute->IsEditable() || !attribute->IsInput())
    {
        return false;
    }

    boost::python::object value = args[1];

    std::any _v = attribute->TypeHandle()->FromPython(value);

    // Set Value.
    return attribute->SetAnyValue(_v);
}


boost::python::object Attribute_Bool(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::Attribute* attribute = boost::python::extract<Gex::Attribute*>(args[0]);

    return boost::python::object(
        attribute != Gex::AttributeWkPtr::invalid.get()
    );
}


boost::python::object At_Node(boost::python::tuple args,
                              boost::python::dict kwargs)
{
    const Gex::AttributePtr& self = boost::python::extract<const Gex::AttributePtr&>(args[0]);

    return boost::python::object(self->Node().ToShared());
}


bool Gex::Python::Attribute_Wrap::RegisterPythonWrapper()
{
    if (registered)
        return false;

    bool (Gex::Attribute::* ProxyAttr_SimpleHasSource)() const = &Gex::Attribute::HasSource;
    bool (Gex::Attribute::* ProxyAttr_HasIndexSource)(unsigned int) = &Gex::Attribute::HasSource;
    Gex::AttributeWkPtr (Gex::Attribute::* ProxyAttr_Source)() const = &Gex::Attribute::Source;
    bool (Gex::Attribute::* ProxyAttr_ConnectSimpleSource)(const Gex::AttributePtr& a) =
    &Gex::Attribute::ConnectSource;
    bool (Gex::Attribute::* ProxyAttr_ConnectWkSource)(const Gex::AttributeWkPtr& a) =
    &Gex::Attribute::ConnectSource;
    bool (Gex::Attribute::* ProxyAttr_ConnectIndexSource)(unsigned int u, const Gex::AttributePtr& a) =
    &Gex::Attribute::ConnectSource;
    bool (Gex::Attribute::* ProxyAttr_ConnectIndexWkSource)(unsigned int u, const Gex::AttributeWkPtr& a) =
    &Gex::Attribute::ConnectSource;
    bool (Gex::Attribute::* ProxyAttr_CanConnectWkSource)(const Gex::AttributeWkPtr& a) =
    &Gex::Attribute::CanConnectSource;
    bool (Gex::Attribute::* ProxyAttr_CanConnectSource)(const Gex::AttributePtr& a) =
    &Gex::Attribute::CanConnectSource;

    boost::python::to_python_converter<
            Gex::AttributeWkPtr,
            Gex::Python::WeakPtrToPython<Gex::Attribute>,
            true>();

    boost::python::class_<Gex::Python::Attribute_Wrap, Gex::AttributePtr>
            ("Attribute", boost::python::no_init)
            .def("Name", &Gex::Attribute::Name)
            .def("Get", boost::python::raw_function(&Attribute_Python_Get))
            .def("Set", boost::python::raw_function(&Attribute_Python_Set, 1))
            .def("GetIndex", &Gex::Attribute::GetIndexAttribute)
//            .def("Node", &Gex::Attribute::Node)
            .def("Node", boost::python::raw_function(&At_Node, 1))
            .def("HasSource", ProxyAttr_SimpleHasSource)
            .def("HasSourceAtIndex", ProxyAttr_HasIndexSource)
            .def("Source", ProxyAttr_Source)
            .def("ConnectSource", ProxyAttr_ConnectSimpleSource)
            .def("ConnectSource", ProxyAttr_ConnectWkSource)
            .def("ConnectSource", ProxyAttr_ConnectIndexSource)
            .def("ConnectSource", ProxyAttr_ConnectIndexWkSource)
            .def("CanConnectSource", ProxyAttr_CanConnectSource)
            .def("CanConnectSource", ProxyAttr_CanConnectWkSource)
            .def("CreateIndex", &Gex::Attribute::CreateIndex)
            .def("IsMulti", &Gex::Attribute::IsMulti)
            .def("ValueHash", &Gex::Attribute::ValueHash)
            .def("ValidIndices", &Gex::Attribute::ValidIndices)
            .def("GetIndexAttribute", &Gex::Attribute::GetIndexAttribute)
            .def("HasChildAttributes", &Gex::Attribute::HasChildAttributes)
            .def("ChildAttributeNames", &Gex::Attribute::ChildAttributesNames)
            .def("GetAttribute", &Gex::Attribute::GetAttribute)
//            .def("ToWeakRef", boost::python::raw_function(&MakeSharedRef, 1))
            .def("__bool__", boost::python::raw_function(Attribute_Bool))
            ;

//    boost::python::class_<Gex::AttributeWkPtr, boost::python::bases<Gex::AttributePtr>>
//            ("AttributeWk", boost::python::no_init)
//            .def("IsWkValid", &Gex::AttributeWkPtr::expired)
//            .def("ToAttribute", &Gex::AttributeWkPtr::lock)
//            .def("__call__", &Gex::AttributeWkPtr::lock)
//            .def("__bool__", &Gex::AttributeWkPtr::operator bool)
//            ;

    boost::python::enum_<Gex::AttrValueType>("AttrValueType")
            .value("Single", Gex::AttrValueType::Single)
            .value("Multi", Gex::AttrValueType::Multi)
            .value("Holder", Gex::AttrValueType::Holder)
            .value("MultiHolder", Gex::AttrValueType::MultiHolder)
            ;

    boost::python::enum_<Gex::AttrType>("AttrType")
            .value("Input", Gex::AttrType::Input)
            .value("Output", Gex::AttrType::Output)
            .value("Static", Gex::AttrType::Static)
            ;

    boost::python::enum_<Gex::AttributeChange>("AttributeChange")
            .value("Connected", Gex::AttributeChange::Connected)
            .value("Disconnected", Gex::AttributeChange::Disconnected)
            .value("ValueChanged", Gex::AttributeChange::ValueChanged)
            .value("IndexAdded", Gex::AttributeChange::IndexAdded)
            .value("IndexAdded", Gex::AttributeChange::IndexAdded)
            .value("IndexRemoved", Gex::AttributeChange::IndexRemoved)
            ;

    registered = true;
    return registered;
}
