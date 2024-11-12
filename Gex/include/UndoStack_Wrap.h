#ifndef GEX_UNDOSTACK_WRAP_H
#define GEX_UNDOSTACK_WRAP_H
#include "boost/python.hpp"
#include "UndoStack.h"
#include "api.h"

namespace Gex::Python
{
    class GEX_API UndoStack_Wrap
    {
        static bool registered;

    public:
        static bool RegisterPythonWrapper();
    };
}

#endif //GEX_UNDOSTACK_WRAP_H
