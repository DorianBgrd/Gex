#ifndef GEX_CONFIG_H
#define GEX_CONFIG_H

#include <string>

namespace Gex
{
    struct Config
    {
    private:
        static Config globalConfig;

    public:
        std::string attributeSeparator = "/";
        std::string attributeSerializedTypeKey = "SerializedAttributeType";
        std::string attributeTypeKey = "AttributeType";
        std::string attributeValueTypeKey = "AttributeValueType";
        std::string attributeInternalKey = "AttributeInternal";
        std::string attributeValueKey = "AttributeValue";
        std::string attributeMultiValueKey = "MultiValues";
        std::string attributeChildrenKey = "ChildAttributes";

        static Config GetConfig()
        {
            return globalConfig;
        };
    };

}


#endif //GEX_CONFIG_H
