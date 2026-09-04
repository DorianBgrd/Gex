#include "Gex/include/Gex/Metadatas.h"


bool Gex::NodeMetadata::SetMetadata(
        const std::string& name,
        const TSys::TypedValuePtr& value
)
{
    if (!IsMetadataType(value))
    {
        return false;
    }

    metadata.emplace(name, value);
    return true;
}


TSys::TypedValuePtr Gex::NodeMetadata::GetMetadata(const std::string& name) const
{
    auto iter = metadata.find(name);
    if (iter == metadata.end())
    {
        return {};
    }

    return iter->second;
}


Gex::JsonValue Gex::NodeMetadata::Serialize(JsonDoc& doc) const
{
    JsonValue serializedMetadata(rapidjson::kArrayType);

    for (const auto& mv : metadata)
    {
        if (!mv.second)
            continue;

        JsonValue valueArray(rapidjson::kArrayType);

        JsonValue& name = rapidjson::Value(rapidjson::kStringType).SetString(
                mv.first.c_str(), doc.GetAllocator());
        JsonValue& type = rapidjson::Value(rapidjson::kStringType).SetString(
                mv.second->GetType()->GetName().c_str(), doc.GetAllocator());
        JsonValue value;

        TSys::GetRegistry()->Serialize(mv.second, value, doc);

        valueArray.PushBack(name.Move(), doc.GetAllocator());
        valueArray.PushBack(type.Move(), doc.GetAllocator());
        valueArray.PushBack(value.Move(), doc.GetAllocator());

        serializedMetadata.PushBack(valueArray.Move(), doc.GetAllocator());
    }

    return serializedMetadata;
}

void Gex::NodeMetadata::Deserialize(JsonValue& val, JsonDoc& doc)
{
    metadata.clear();

    auto* registry = TSys::GetRegistry();

    for (unsigned int i = 0;i < val.Size(); i++)
    {
        std::string name = val[i][0].GetString();
        std::string type = val[i][1].GetString();
        rapidjson::Value& metadataValue = val[i][2];

        auto valueType = registry->GetType(name);
        if (!valueType)
            continue;

        TSys::TypedValuePtr deserializedValue = registry->CreateValue(valueType);

        TSys::GetRegistry()->DeserializeValue(
                deserializedValue, metadataValue, doc
        );

        metadata[name] = deserializedValue;
    }
}


Gex::NodeMetadata Gex::NodeMetadata::DeserializeMetadata(JsonValue val, JsonDoc& doc)
{
    Gex::NodeMetadata m;
    m.Deserialize(val, doc);
    return m;
}

