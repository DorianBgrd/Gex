#ifndef GEX_NODEFACTORY_WRAP_H
#define GEX_NODEFACTORY_WRAP_H

#include "api.h"
#include "PluginLoader.h"

namespace Gex
{
    namespace Python
    {
        struct GEX_API NodeBuilder_Wrap: public DefaultNodeBuilder,
                public boost::python::wrapper<DefaultNodeBuilder>
        {
        private:
            static bool pythonRegistered;
        public:
            NodeBuilder_Wrap();

            NodeBuilder_Wrap(const DefaultNodeBuilder base);

            Node* CreateNode() const override;

        public:
            static bool RegisterPythonWrapper();
        };


        struct GEX_API NodeFactory_Wrap
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}

#endif //GEX_NODEFACTORY_WRAP_H
