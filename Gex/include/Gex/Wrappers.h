#ifndef GEX_PYTHON_WRAPPERS_H
#define GEX_PYTHON_WRAPPERS_H
#include "api.h"

#include "pybind11/pybind11.h"

namespace Gex::Python
{
    GEX_API
    void RegisterPythonWrappers(pybind11::module_& mod,
                                PyThreadState* state);

    GEX_API
    void RegisterLocalModule(PyThreadState* state=nullptr);
}

#endif //GEX_PYTHON_WRAPPERS_H
