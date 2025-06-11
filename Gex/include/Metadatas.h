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

#include "Tsys/include/tsys.h"

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
        return bool(TSys::TypeRegistry::GetRegistry()->IsRegistered(typeid(T)));
    }

    inline bool IsMetadataType(const std::any& value)
    {
        return bool(TSys::TypeRegistry::GetRegistry()->GetTypeHandle(value));
    }


    class GEX_API NodeMetadata
    {
        std::unordered_map<std::string, std::any> metadata;

    public:
        NodeMetadata() = default;

        bool SetMetadata(const std::string& name,
                         const std::any& value);

        std::any GetMetadata(const std::string& name) const;

        template<class T>
        bool Set(const std::string& name, T value)
        {
            return SetMetadata(name, value);
        }

        template<class T>
        T Get(const std::string& name, Feedback* res=nullptr) const
        {
            auto val = GetMetadata(name);
            if (!val.has_value())
            {
                RETURN_WITH_STATUS(Status::Failed, "No value named " + name, {})
            }

            if (std::type_index(typeid(T)) == val.type())
            {
                RETURN_WITH_STATUS(Status::Success, "", std::any_cast<T>(val))
            }

            auto handle = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(val);
            if (handle->CanConvertFrom(val.type().hash_code()))
            {
                RETURN_WITH_STATUS(Status::Success, "", std::any_cast<T>(handle->ConvertFrom(val, val)))
            }

            RETURN_WITH_STATUS(Status::Failed, "Invalid metadata type.", {})
        }

        JsonValue Serialize(JsonDoc& doc) const;

        void Deserialize(JsonValue& val);

        static NodeMetadata DeserializeMetadata(JsonValue val);
    };
}

#endif //GEX_METADATAS_H
