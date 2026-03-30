#include "Gex/include/NodeAttributeData_Wrap.h"

#include "boost/python.hpp"

#include "Tsys/defaultTypes.h"


bool Gex::Python::NodeAttributeData_Wrap::pythonRegistered = false;


boost::python::object NAD_GetValue(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::NodeAttributeData* context = boost::python::extract<Gex::NodeAttributeData*>(args[0]);

    auto handle = TSys::TypeRegistry::GetRegistry()
            ->GetTypeHandle(
                    context->GetAnyValue()
            );

    if (!handle)
        return {};

    return handle->ToPython(context->GetAnyValue());
}


boost::python::object NAD_GetIndexValue(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::NodeAttributeData* context = boost::python::extract<Gex::NodeAttributeData*>(args[0]);

    int index = boost::python::extract<int>(args[1]);

    auto handle = TSys::TypeRegistry::GetRegistry()
            ->GetTypeHandle(
                    context->GetIndex(index).GetAnyValue()
            );

    if (!handle)
        return {};

    return handle->ToPython(context->GetAnyValue());
}


boost::python::object NAD_SetValue(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::NodeAttributeData* context = boost::python::extract<Gex::NodeAttributeData*>(args[0]);

    boost::python::object value = args[1];

    int size = boost::python::len(args);

    int pval = PyLong_AsLong(value.ptr());

    int val = boost::python::extract<int>(value);

    auto val2 = TSys::ExtractPythonToAny<int>(value);

    bool success = context->SetValue(value);

    return {};
}


boost::python::object NAD_SetIndexValue(
        boost::python::tuple args,
        boost::python::dict kwargs
)
{
    Gex::NodeAttributeData* context = boost::python::extract<Gex::NodeAttributeData*>(args[0]);

    int index = boost::python::extract<int>(args[1]);

    TSys::AnyValue value;
    value.Python_Set(args[2]);

    return boost::python::object(context->SetValue(value));
}


bool Gex::Python::NodeAttributeData_Wrap::RegisterPythonWrapper()
{
    if (pythonRegistered)
        return false;

    boost::python::class_<Gex::NodeAttributeData>("NodeAttributeData", boost::python::no_init)
        .def("GetAttribute", &Gex::NodeAttributeData::GetAttribute,
             (boost::python::arg("attribute"),
              boost::python::args("feedback")=boost::python::object()))
        .def("GetNode", &Gex::NodeAttributeData::GetNode)
        .def("GetIndex", &Gex::NodeAttributeData::GetIndex)
        .def("GetValue", boost::python::raw_function(&NAD_GetValue, 1))
        .def("SetValue", boost::python::raw_function(&NAD_SetValue, 2))
        .def("GetIndexValue", boost::python::raw_function(&NAD_GetIndexValue, 1))
        .def("SetIndexValue", boost::python::raw_function(&NAD_SetIndexValue, 2));

    pythonRegistered = true;
    return true;
}