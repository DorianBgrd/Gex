#include "Gex/include/Wrappers.h"

#include "Tsys/tsys.h"

#include "pybind11/pybind11.h"


pybind11::object GetAvailableTypes(
        pybind11::args args,
        pybind11::kwargs kwargs
)
{
    pybind11::list types;
    for (const std::string& t : TSys::TypeRegistry::GetRegistry()->RegisteredTypes())
    {
        types.append(pybind11::cast(t));
    }

    return types;
}


PYBIND11_MODULE(Gex_Python, mod, pybind11::multiple_interpreters::per_interpreter_gil())
{

    mod.def(
            "GetAvailableTypes",
            &GetAvailableTypes
    );


    Gex::Python::RegisterPythonWrappers(mod, nullptr);

}