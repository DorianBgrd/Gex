#ifndef GEX_PYTHON_IO_H
#define GEX_PYTHON_IO_H

#include "api.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API IO
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}


#endif //GEX_PYTHON_IO_H
