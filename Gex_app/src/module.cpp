#include "Gex_app/include/PythonOutput.h"
#include "Gex_app/include/AppContext.h"

#include "pybind11/pybind11.h"




PYBIND11_MODULE(SoftPython, mod, pybind11::multiple_interpreters::per_interpreter_gil())
{
    SoftPython::PythonOutput::RegisterPythonWrapper(mod);

    App::AppContext::RegisterPythonWrapper(mod);

    App::SelectionContext::RegisterPythonWrapper(mod);
}
