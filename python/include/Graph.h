#ifndef GEX_GRAPH_H
#define GEX_GRAPH_H

#include "api.h"
#include "include/Graph.h"


namespace Gex
{
    namespace Python
    {
        struct GEX_PYTHON_API GraphContext_Wrap: public Gex::GraphContext
        {
        private:
            static bool pythonRegistered;

        public:
            static bool RegisterPythonWrapper();
        };


        struct GEX_PYTHON_API Graph_Wrap: public Gex::Graph
        {
        private:
            static bool pythonRegistered;

        public:
            static bool RegisterPythonWrapper();
        };
    }
}

#endif //GEX_GRAPH_H
