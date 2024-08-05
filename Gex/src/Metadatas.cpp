#include "Gex/include/Metadatas.h"


Gex::JsonValue Gex::NodeMetadata::Serialize(JsonDoc& doc) const
{
    JsonValue serializedMetadata(rapidjson::kArrayType);

    for (auto mv : metadata)
    {
        std::any anyval = mv.second;
        auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(
                anyval.type().hash_code());
        if (!handler)
            continue;

        JsonValue valueArray(rapidjson::kArrayType);

        JsonValue& name = rapidjson::Value(rapidjson::kStringType).SetString(
                mv.first.c_str(), doc.GetAllocator());
        JsonValue& type = rapidjson::Value(rapidjson::kStringType).SetString(
                handler->ApiName().c_str(), doc.GetAllocator());
        JsonValue& value = rapidjson::Value().SetArray();

        handler->SerializeValue(anyval, value, doc);

        valueArray.PushBack(name.Move(), doc.GetAllocator());
        valueArray.PushBack(type.Move(), doc.GetAllocator());
        valueArray.PushBack(value.Move(), doc.GetAllocator());

        serializedMetadata.PushBack(valueArray.Move(), doc.GetAllocator());
    }

    return serializedMetadata;
}

void Gex::NodeMetadata::Deserialize(JsonValue& val)
{
    metadata.clear();

    for (unsigned int i = 0;i < val.Size(); i++)
    {
        std::string name = val[i][0].GetString();
        std::string type = val[i][1].GetString();
        rapidjson::Value& m = val[i][2];

        auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandleFromApiName(type);
        if (!handler)
            continue;

        std::any n;
        metadata[name] = handler->DeserializeValue(n, m);
    }
}


Gex::NodeMetadata Gex::NodeMetadata::DeserializeMetadata(JsonValue val)
{
    Gex::NodeMetadata m;
    m.Deserialize(val);
    return m;
}

