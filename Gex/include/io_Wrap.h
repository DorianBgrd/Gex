#ifndef GEX_PYTHON_IO_H
#define GEX_PYTHON_IO_H

#include "api.h"
#include "Gex/python/utils.h"

#include "pybind11/pybind11.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API IO
        {
        private:
            static bool pythonRegistered;
            static PyClassRegistry registry;

        public:
            static bool RegisterPythonWrapper(pybind11::module_& mod,
                                              PyThreadState* state=nullptr);

            static bool IsRegistered();
        };
    }
}


#endif //GEX_PYTHON_IO_H
