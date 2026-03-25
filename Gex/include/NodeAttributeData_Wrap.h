#ifndef GEX_NODEATTRIBUTEDATA_WRAP_H
#define GEX_NODEATTRIBUTEDATA_WRAP_H

#include "Node.h"
#include "Attribute.h"
#include "NodeAttributeData.h"

namespace Gex::Python
{
    struct GEX_API NodeAttributeData_Wrap
    {
    private:
        static bool pythonRegistered;
    public:
        static bool RegisterPythonWrapper();
    };
}

#endif //GEX_NODEATTRIBUTEDATA_WRAP_H
