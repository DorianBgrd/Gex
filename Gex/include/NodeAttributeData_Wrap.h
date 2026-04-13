#ifndef GEX_NODEATTRIBUTEDATA_WRAP_H
#define GEX_NODEATTRIBUTEDATA_WRAP_H

#include "Node.h"
#include "Attribute.h"
#include "NodeAttributeData.h"

#include "Gex/python/utils.h"

namespace Gex::Python
{
    struct GEX_API NodeAttributeData_Wrap
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

#endif //GEX_NODEATTRIBUTEDATA_WRAP_H
