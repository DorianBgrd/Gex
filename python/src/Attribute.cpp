#include "python/include/Attribute.h"
#include "Gex/include/Node.h"
#include "Tsys/include/tsys.h"

bool Gex::Python::Attribute_Wrap::registered = false;


boost::python::object Attribute_Python_Get(boost::python::tuple args,
                                           boost::python::dict kwargs)
{
    boost::python::object result;
    Gex::Attribute* attribute = boost::python::extract<Gex::Attribute*>(args[0]);

    TSys::TypeHandler* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(attribute->TypeHash());
    if (handler == nullptr)
    {
        return result;
    }

    return handler->ToPython(attribute->GetAnyValue());
}


bool Attribute_Python_Set(boost::python::tuple args,
                          boost::python::dict kwargs)
{
    Gex::Attribute* attribute = boost::python::extract<Gex::Attribute*>(args[0]);

    if (!attribute->IsEditable())
        return false;

    if (attribute->IsOutput())
    {
        return false;
    }

    boost::python::object value = args[1];
    std::string classname = boost::python::extract<std::string>(
            value.attr("__class__").attr("__name__"));

    TSys::TypeHandler* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandleFromPythonName(classname);
    if (!handler)
    {
        return false;
    }

    std::any _v = handler->FromPython(value);

    // Set Value.
    return attribute->SetAnyValue(value);
}


bool Gex::Python::Attribute_Wrap::RegisterPythonWrapper()
{
    bool (Gex::Attribute::* ProxyAttr_SimpleHasSource)() const = &Gex::Attribute::HasSource;
    bool (Gex::Attribute::* ProxyAttr_HasIndexSource)(unsigned int) = &Gex::Attribute::HasSource;
    Gex::Attribute* (Gex::Attribute::* ProxyAttr_Source)() const = &Gex::Attribute::Source;
    bool (Gex::Attribute::* ProxyAttr_ConnectSimpleSource)(Gex::Attribute* a) =
    &Gex::Attribute::ConnectSource;
    bool (Gex::Attribute::* ProxyAttr_ConnectIndexSource)(unsigned int u, Gex::Attribute* a) =
    &Gex::Attribute::ConnectSource;
    boost::python::class_<Gex::Python::Attribute_Wrap, boost::noncopyable>("Attribute", boost::python::no_init)
            .def("Name", &Gex::Attribute::Name)
            .def("Get", boost::python::raw_function(&Attribute_Python_Get))
            .def("Set", boost::python::raw_function(&Attribute_Python_Set, 1))
            .def("GetIndex", &Gex::Attribute::GetIndexAttribute,
                 boost::python::return_internal_reference())
            .def("Node", &Gex::Attribute::Node,
                 boost::python::return_internal_reference())
            .def("HasSource", ProxyAttr_SimpleHasSource)
            .def("HasSourceAtIndex", ProxyAttr_HasIndexSource)
            .def("Source", ProxyAttr_Source, boost::python::return_internal_reference())
            .def("ConnectSource", ProxyAttr_ConnectSimpleSource)
            .def("ConnectSource", ProxyAttr_ConnectIndexSource)
//            .def("ConnectDest", &Gex::Attribute::ConnectDest)
            .def("CanConnectSource", &Gex::Attribute::CanConnectSource)
            .def("CreateIndex", &Gex::Attribute::CreateIndex)
            .def("IsMulti", &Gex::Attribute::IsMulti)
            .def("ValueHash", &Gex::Attribute::ValueHash)
            .def("ValidIndices", &Gex::Attribute::ValidIndices)
            .def("GetIndexAttribute", &Gex::Attribute::GetIndexAttribute,
                 boost::python::return_internal_reference())
            .def("HasChildAttributes", &Gex::Attribute::HasChildAttributes)
            .def("ChildAttributeNames", &Gex::Attribute::ChildAttributesNames)
            .def("GetAttribute", &Gex::Attribute::GetAttribute,
                 boost::python::return_internal_reference())
            ;


    boost::python::enum_<Gex::AttrValueType>("AttrValueType")
            .value("Single", Gex::AttrValueType::Single)
            .value("Multi", Gex::AttrValueType::Multi)
            .value("Holder", Gex::AttrValueType::Holder)
            .value("MultiHolder", Gex::AttrValueType::MultiHolder)
            .export_values();

    boost::python::enum_<Gex::AttrType>("AttrType")
            .value("Input", Gex::AttrType::Input)
            .value("Output", Gex::AttrType::Output)
            .value("Static", Gex::AttrType::Static)
            .export_values();

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
