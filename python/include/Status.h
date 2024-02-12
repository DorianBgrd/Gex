#ifndef GEX_STATUS_H
#define GEX_STATUS_H

#include "api.h"
#include "include/Status.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_PYTHON_API Feedback_Wrap: public Gex::Feedback
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}




#endif //GEX_STATUS_H
