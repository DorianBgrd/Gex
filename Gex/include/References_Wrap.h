#ifndef GEX_REFERENCES_WRAP_H
#define GEX_REFERENCES_WRAP_H

#include "Gex/include/References.h"
#include "api.h"

namespace Gex::Python
{
    struct GEX_API References_Wrap
    {
    private:
        static bool registered;
    public:
        static bool RegisterPythonWrapper();
    };
}

#endif //GEX_REFERENCES_WRAP_H
