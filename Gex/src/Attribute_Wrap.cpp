#include "Gex/include/Gex/Attribute_Wrap.h"
#include "Gex/include/Gex/Node.h"
#include "Tsys/tsys.h"

#include "Gex/include/Gex/ptrs.h"

#include "pybind11/pybind11.h"

bool Gex::Python::Attribute_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::Attribute_Wrap::registry;


pybind11::object Attribute_Python_Get(pybind11::args args,
                                      pybind11::kwargs kwargs)
{
    Gex::Attribute* attribute = args[0].cast<Gex::Attribute*>();
    if (!attribute)
    {
        return {};
    }

    return attribute->TypeHandle()->ToPython(attribute->GetAnyValue());
}


bool Attribute_Python_Set(pybind11::args args,
                          pybind11::kwargs kwargs)
{
    Gex::Attribute* attribute = args[0].cast<Gex::Attribute*>();
    if (!attribute)
    {
        return false;
    }

    if (!attribute->IsEditable() || !attribute->IsInput())
    {
        return false;
    }

    pybind11::object value = args[1];

    std::any _v = attribute->TypeHandle()->FromPython(value);

    // Set Value.
    return attribute->SetAnyValue(_v);
}


pybind11::object Attribute_Bool(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::Attribute* attribute = args[0].cast<Gex::Attribute*>();

    return pybind11::cast(
        attribute != Gex::AttributeWkPtr::invalid.get()
    );
}


pybind11::object At_Node(pybind11::args args,
                         pybind11::kwargs kwargs)
{
    const Gex::AttributePtr& self = args[0].cast<const Gex::AttributePtr&>();

    return pybind11::cast(self->Node().ToShared());
}


bool Gex::Python::Attribute_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                        PyThreadState* state)
{
    if(registry.IsRegistered(state))
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

    pybind11::class_<Gex::Attribute, Gex::AttributePtr>(mod, "Attribute", pybind11::module_local(false))
            .def("Name", &Gex::Attribute::Name)
            .def("Get", &Attribute_Python_Get)
            .def("Set", &Attribute_Python_Set)
            .def("GetIndex", &Gex::Attribute::GetIndexAttribute)
//            .def("Node", &Gex::Attribute::Node)
            .def("Node", &At_Node)
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
//            .def("ToWeakRef", pybind11::raw_function(&MakeSharedRef, 1))
            .def("__bool__", &Attribute_Bool)
            ;

//    pybind11::class_<Gex::AttributeWkPtr, pybind11::bases<Gex::AttributePtr>>
//            ("AttributeWk", pybind11::no_init)
//            .def("IsWkValid", &Gex::AttributeWkPtr::expired)
//            .def("ToAttribute", &Gex::AttributeWkPtr::lock)
//            .def("__call__", &Gex::AttributeWkPtr::lock)
//            .def("__bool__", &Gex::AttributeWkPtr::operator bool)
//            ;

    pybind11::enum_<Gex::AttrValueType>(mod, "AttrValueType", pybind11::module_local(false))
            .value("Single", Gex::AttrValueType::Single)
            .value("Multi", Gex::AttrValueType::Multi)
            .value("Holder", Gex::AttrValueType::Holder)
            .value("MultiHolder", Gex::AttrValueType::MultiHolder)
            ;

    pybind11::enum_<Gex::AttrType>(mod, "AttrType", pybind11::module_local(false))
            .value("Input", Gex::AttrType::Input)
            .value("Output", Gex::AttrType::Output)
            .value("Static", Gex::AttrType::Static)
            ;

    pybind11::enum_<Gex::AttributeChange>(mod, "AttributeChange", pybind11::module_local(false))
            .value("Connected", Gex::AttributeChange::Connected)
            .value("Disconnected", Gex::AttributeChange::Disconnected)
            .value("ValueChanged", Gex::AttributeChange::ValueChanged)
            .value("IndexAdded", Gex::AttributeChange::IndexAdded)
            .value("IndexRemoved", Gex::AttributeChange::IndexRemoved)
            ;

    return registry.Register(state);
}


bool Gex::Python::Attribute_Wrap::IsRegistered()
{
    return registry.IsRegistered();
}
