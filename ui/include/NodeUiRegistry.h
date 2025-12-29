#ifndef GEX_NODEUIREGISTRY_H
#define GEX_NODEUIREGISTRY_H

#include <QIcon>


namespace Gex::Ui
{
    class NodeUiRegistry
    {

    private:
        struct NodeResource
        {
            QIcon icon;
        };

        std::unordered_map<std::string, NodeResource> resources;

        static NodeUiRegistry* registry;

    public:
        NodeUiRegistry();

        void RegisterNodeIcon(
                const std::string& nodeType,
                const QIcon& icon
        );

        QIcon GetNodeIcon(
                const std::string& nodeType
        );

        static NodeUiRegistry* GetRegistry();
    };
}

#endif //GEX_NODEUIREGISTRY_H
