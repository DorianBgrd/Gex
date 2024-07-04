#ifndef GEX_PYTHON_STATUS_H
#define GEX_PYTHON_STATUS_H

#include "api.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API Feedback_Wrap
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}




#endif //GEX_PYTHON_STATUS_H
