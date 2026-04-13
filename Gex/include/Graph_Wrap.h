#ifndef GEX_GRAPH_WRAP_H
#define GEX_GRAPH_WRAP_H

#include "api.h"
#include "Node.h"
#include "Gex/python/utils.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_API GraphContext_Wrap: public Gex::GraphContext
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

#endif //GEX_GRAPH_WRAP_H
