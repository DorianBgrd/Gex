#ifndef GEX_JSON_TYPES_H
#define GEX_JSON_TYPES_H

#include <memory>
#include <vector>

#include "Tsys/tsys.h"
#include "rapidjson/document.h"

#include "jsonDefs.h"

namespace JsonPlugin
{
    class JsonValueReader
    {
    private:
        rapidjson::Value value;
        rapidjson::Document::AllocatorType alloc;

    public:
        JsonValueReader() = default;

        JsonValueReader(rapidjson::Value& value,
                        rapidjson::Document& document)
        {
            alloc = document.GetAllocator();
            value.CopyFrom(value, alloc);
        }

        JsonValueReader(const JsonValueReader& other)
        {
            alloc = other.alloc;
            value.CopyFrom(other.value, alloc);
        }

        rapidjson::Value& Get()
        {
            return value;
        }
    };


    void SerializeJsonValue(
            const TSys::TypedValuePtr& v,
            rapidjson::Value& value,
            rapidjson::Document& document
    );

    void DeserializeJsonValue(
            const TSys::TypedValuePtr& v,
            rapidjson::Value& value,
            rapidjson::Document&
    );

    void SerializeJsonValueReader(
            const TSys::TypedValuePtr& v,
            rapidjson::Value& value,
            rapidjson::Document& document
    );

    void DeserializeJsonValueReader(
            const TSys::TypedValuePtr& v,
            rapidjson::Value& value,
            rapidjson::Document&
    );

    void SerializeJsonValueEditor(
            const TSys::TypedValuePtr& v,
            rapidjson::Value& value,
            rapidjson::Document& document
    );

    void DeserializeJsonValueEditor(
            const TSys::TypedValuePtr& v,
            rapidjson::Value& value,
            rapidjson::Document&
    );

}


#endif //GEX_JSON_TYPES_H
