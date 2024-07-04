#ifndef GEX_GRAPH_WRAP_H
#define GEX_GRAPH_WRAP_H

#include "api.h"
#include "Node.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API GraphContext_Wrap: public Gex::GraphContext
        {
        private:
            static bool pythonRegistered;

        public:
            static bool RegisterPythonWrapper();
        };
    }
}

#endif //GEX_GRAPH_WRAP_H
