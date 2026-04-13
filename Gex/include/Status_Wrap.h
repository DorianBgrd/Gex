#ifndef GEX_PYTHON_STATUS_H
#define GEX_PYTHON_STATUS_H

#include "api.h"
#include "pybind11/pybind11.h"

#include "Gex/python/utils.h"

namespace Gex
{
    namespace Python
    {
        struct GEX_API Feedback_Wrap
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




#endif //GEX_PYTHON_STATUS_H
