#ifndef GEX_NODEFACTORY_WRAP_H
#define GEX_NODEFACTORY_WRAP_H

#include "api.h"
#include "PluginLoader.h"
#include "Gex/python/utils.h"

#include "pybind11/pybind11.h"

namespace Gex
{
    namespace Python
    {
        struct GEX_API NodeBuilder_Wrap: public DefaultNodeBuilder,
                public pybind11::trampoline_self_life_support
        {
        private:
            static bool pythonRegistered;
            static PyClassRegistry registry;

        public:
            NodeBuilder_Wrap();

            NodeBuilder_Wrap(const DefaultNodeBuilder base);

            Node* CreateNode() const override;

        public:
            static bool RegisterPythonWrapper(pybind11::module_& mod,
                                              PyThreadState* state=nullptr);

            static bool IsRegistered();
        };


        struct GEX_API NodeFactory_Wrap
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

#endif //GEX_NODEFACTORY_WRAP_H
