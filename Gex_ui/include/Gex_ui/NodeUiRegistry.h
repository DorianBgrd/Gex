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
            QColor color;
        };

        std::unordered_map<std::string, NodeResource> resources;

        static NodeUiRegistry* registry;

        QColor defaultColor;

    public:
        NodeUiRegistry();

        void RegisterNodeResource(
                const std::string& nodeType,
                const QIcon& icon,
                const QColor& color
        );

        QIcon GetNodeIcon(
                const std::string& nodeType
        );

        QColor GetNodeColor(
                const std::string& nodeType
        );

        NodeResource GetNodeResource(
                const std::string& nodeType
        );

        static NodeUiRegistry* GetRegistry();
    };
}

#endif //GEX_NODEUIREGISTRY_H
