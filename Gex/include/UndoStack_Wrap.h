#ifndef GEX_UNDOSTACK_WRAP_H
#define GEX_UNDOSTACK_WRAP_H
#include "UndoStack.h"
#include "api.h"

#include "pybind11/pybind11.h"

#include "Gex/python/utils.h"

namespace Gex::Python
{
    class GEX_API UndoStack_Wrap
    {
        static bool registered;
        static PyClassRegistry registry;

    public:
        static bool RegisterPythonWrapper(pybind11::module_& mod,
                                          PyThreadState* state=nullptr);

        static bool IsRegistered();
    };
}

#endif //GEX_UNDOSTACK_WRAP_H
