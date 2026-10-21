#ifndef GEX_METADATAS_H
#define GEX_METADATAS_H

#include "api.h"
#include "Status.h"

#include <variant>
#include <vector>
#include <string>
#include <typeinfo>
#include <memory>
#include <any>

#include "rapidjson/document.h"

#include "Tsys/tsys.h"

#define M_TYPES int, float, double, std::string, std::vector<int>, std::vector<float>, std::vector<double>,
#define IsAvailableType(T)

#define RETURN_WITH_STATUS(stat, msg, result) \
if (res) \
    res->Set(stat, msg); \
return result; \

namespace Gex
{
    typedef rapidjson::Value JsonValue;
    typedef rapidjson::Document JsonDoc;


    template<class T>
    inline bool IsMetadataType()
    {
        return bool(TSys::GetRegistry()->GetType<T>());
    }

    inline bool IsMetadataType(const std::any& value)
    {
        return bool(TSys::GetRegistry()->GetType(value));
    }


    class GEX_API NodeMetadata
    {
        std::unordered_map<std::string, TSys::TypedValuePtr> metadata;

    public:
        NodeMetadata() = default;

        bool SetMetadata(const std::string& name,
                         const TSys::TypedValuePtr& value);


        TSys::TypedValuePtr GetMetadata(const std::string& name) const;

        template<class T>
        bool Set(const std::string& name, T value)
        {
            return SetMetadata(name, TSys::GetRegistry()->WrapValue(value));
        }

        template<class T>
        T Get(const std::string& name, Feedback* res=nullptr) const
        {
            auto val = GetMetadata(name);
            if (!val)
            {
                RETURN_WITH_STATUS(Status::Failed, "No value named " + name, {})
            }

            if (!val->GetType()->IsConvertibleTo<T>())
            {
                RETURN_WITH_STATUS(Status::Failed, "Invalid metadata type.", {})
            }

            return val->Get<T>();
        }

        JsonValue Serialize(JsonDoc& doc) const;

        void Deserialize(JsonValue& val, JsonDoc& doc);

        static NodeMetadata DeserializeMetadata(JsonValue val, JsonDoc& doc);
    };
}

#endif //GEX_METADATAS_H
