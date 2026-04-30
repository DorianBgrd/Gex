#ifndef GEX_REFERENCES_WRAP_H
#define GEX_REFERENCES_WRAP_H

#include "References.h"
#include "api.h"

#include "pybind11/pybind11.h"

#include "Gex/python/utils.h"

namespace Gex::Python
{
    struct GEX_API References_Wrap
    {
    private:
        static bool registered;
        static PyClassRegistry registry;

    public:
        static bool RegisterPythonWrapper(pybind11::module_& mod,
                                          PyThreadState* state=nullptr);

        static bool IsRegistered();
    };
}

#endif //GEX_REFERENCES_WRAP_H
