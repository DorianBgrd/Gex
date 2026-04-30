#include "Gex/include/Gex/References_Wrap.h"


bool Gex::Python::References_Wrap::registered = false;
Gex::Python::PyClassRegistry Gex::Python::References_Wrap::registry;



pybind11::object Python_ReferencesGetLoader(
        pybind11::args args,
        pybind11::kwargs kwargs)
{
    auto* refs = Gex::References::GetLoader();

    return pybind11::cast(refs);
}


pybind11::object Python_ReferencesAvailableRefs(
        pybind11::args args,
        pybind11::kwargs kwargs)
{
    pybind11::list dirs;

    Gex::References* refs = args[0].cast<Gex::References*>();

    for (auto d : refs->GetAvailableReferences())
    {
        dirs.append(d);
    }

    return dirs;
}


pybind11::object Python_ReferencesGetDirectories(
        pybind11::args args,
        pybind11::kwargs kwargs)
{
    pybind11::list dirs;

    Gex::References* refs = args[0].cast<Gex::References*>();

    for (auto d : refs->GetDirectories())
    {
        dirs.append(d);
    }

    return dirs;
}


bool Gex::Python::References_Wrap::RegisterPythonWrapper(pybind11::module_& mod,
                                                         PyThreadState* state)
{
    if (registry.IsRegistered(state))
        return false;

    pybind11::class_<Gex::References>(mod, "References", pybind11::module_local(false))
            .def_static("GetLoader", &Python_ReferencesGetLoader)
             .def("AddDirectory", &Gex::References::AddDirectory)
             .def("RemDirectory", &Gex::References::RemDirectory)
             .def("GetAvailableReferences", &Python_ReferencesAvailableRefs)
             .def("GetDirectories", &Python_ReferencesGetDirectories)
             .def("ResolvePath", &Gex::References::ResolvePath)
             ;

    return registry.Register(state);
}


bool Gex::Python::References_Wrap::IsRegistered()
{
    return registered;
}