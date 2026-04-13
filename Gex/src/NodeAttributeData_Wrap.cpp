#include "Gex/include/NodeAttributeData_Wrap.h"

#include "Tsys/defaultTypes.h"


bool Gex::Python::NodeAttributeData_Wrap::pythonRegistered = false;
Gex::Python::PyClassRegistry Gex::Python::NodeAttributeData_Wrap::registry;


pybind11::object NAD_GetValue(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::NodeAttributeData* context = args[0].cast<Gex::NodeAttributeData*>();

    auto handle = TSys::TypeRegistry::GetRegistry()
            ->GetTypeHandle(
                    context->GetAnyValue()
            );

    if (!handle)
        return {};

    return handle->ToPython(context->GetAnyValue());
}


pybind11::object NAD_GetIndexValue(
        pybind11::tuple args,
        pybind11::dict kwargs
)
{
    Gex::NodeAttributeData* context = args[0].cast<Gex::NodeAttributeData*>();

    int index = args[1].cast<int>();

    auto handle = TSys::TypeRegistry::GetRegistry()
            ->GetTypeHandle(
                    context->GetIndex(index).GetAnyValue()
            );

    if (!handle)
        return {};

    return handle->ToPython(context->GetAnyValue());
}


pybind11::object NAD_SetValue(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    Gex::NodeAttributeData* context = args[0].cast<Gex::NodeAttributeData*>();

    pybind11::object value = args[1];

    context->SetValue(value);

    return {};
}


pybind11::object NAD_SetIndexValue(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    Gex::NodeAttributeData* context = args[0].cast<Gex::NodeAttributeData*>();

    int index = args[1].cast<int>();

    TSys::AnyValue value;
    context->GetIndex(index).SetValue(args[2]);

    return pybind11::cast(context->SetValue(value));
}


bool Gex::Python::NodeAttributeData_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                                PyThreadState* state)
{
//    if (pythonRegistered)
//        return false;
    auto reg = registry;
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Gex::NodeAttributeData>(mod, "NodeAttributeData", pybind11::module_local(false))
        .def("GetAttribute", &Gex::NodeAttributeData::GetAttribute)
        .def("GetNode", &Gex::NodeAttributeData::GetNode)
        .def("GetIndex", &Gex::NodeAttributeData::GetIndex)
        .def("GetValue", &NAD_GetValue)
        .def("SetValue", &NAD_SetValue)
        .def("GetIndexValue", &NAD_GetIndexValue)
        .def("SetIndexValue", &NAD_SetIndexValue);

    return registry.Register(state);
}


bool Gex::Python::NodeAttributeData_Wrap::IsRegistered()
{
    return registry.IsRegistered();
}