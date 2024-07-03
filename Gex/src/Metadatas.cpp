#include "Gex/include/Metadatas.h"


Gex::Metadata::Metadata(size_t h)
{
    hash = h;
}


bool Gex::Metadata::ValueSet(std::any val)
{
    if (val.type().hash_code() != hash)
    {
        return false;
    }

    value = val;
    return true;
}


std::any Gex::Metadata::ValueGet(const size_t& h)
{
    if (!value.has_value())
        return {};

    if (value.type().hash_code() == hash)
    {
        return value;
    }
    else
    {
        TSys::TypeHandler* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(h);
        if (!handler->CanConvertFrom(value))
        {
            return {};
        }

        return handler->ConvertFrom(value, value);
    }
}


bool Gex::Metadata::Serialize(JsonValue json, JsonDoc doc)
{
    TSys::TypeHandler* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(hash);
    if (!handler || !value.has_value())
    {
        return true;
    }

    handler->SerializeValue(value, json, doc);
    return false;
}


bool Gex::Metadata::Deserialize(JsonValue json)
{

}
