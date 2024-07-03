#ifndef GEX_NODEFACTORY_H
#define GEX_NODEFACTORY_H

#include "Gex/include/PluginLoader.h"

namespace Gex
{
    namespace Python
    {
        struct NodeBuilder_Wrap: public DefaultNodeBuilder,
                public boost::python::wrapper<DefaultNodeBuilder>
        {
            NodeBuilder_Wrap();

            NodeBuilder_Wrap(const DefaultNodeBuilder base);

            Node* CreateNode() const override;
        };


        struct NodeFactory_Wrap
        {
        private:
            static bool pythonRegistered;
        public:
            static bool RegisterPythonWrapper();
        };
    }
}

#endif //GEX_NODEFACTORY_H
