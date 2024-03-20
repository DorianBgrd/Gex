#ifndef GEX_CONFIG_H
#define GEX_CONFIG_H

#include "api.h"
#include <string>

namespace Gex
{
    struct GEX_API Config
    {
    private:
        static Config globalConfig;

    public:
        std::string nodePathSeparator = "/";

        std::string attributeSeparator = ".";
        std::string attributeSerializedTypeKey = "SerializedAttributeType";
        std::string attributeTypeKey = "AttributeType";
        std::string attributeValueTypeKey = "AttributeValueType";
        std::string attributeInternalKey = "AttributeInternal";
        std::string attributeValueKey = "AttributeValue";
        std::string attributeMultiValueKey = "MultiValues";
        std::string attributeChildrenKey = "ChildAttributes";

        std::string nodeTypeKey = "Type";
        std::string nodeNameKey = "Name";
        std::string nodeAttributesKey = "Attributes";
        std::string nodeCustomAttributesKey = "CustomAttributes";

        std::string compoundNodesKey = "Nodes";
        std::string compoundConnectionsKey = "Connections";
        std::string compoundPluginsKey = "Plugins";
        std::string compoundNodeTypes = "NodeTypes";

        std::string thisKey = "This";

        static Config GetConfig();
    };

}


#endif //GEX_CONFIG_H
