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

namespace Gex
{
    typedef rapidjson::Value JsonValue;
    typedef rapidjson::Document JsonDoc;


    template<class T>
    bool IsMetadataType()
    {
        return bool(TSys::TypeRegistry::GetRegistry()->IsRegistered(
                typeid(T).hash_code()));
    }


    class GEX_API NodeMetadata
    {
        std::unordered_map<std::string, std::any> metadata;

    public:
        NodeMetadata() = default;

        template<class T>
        bool SetMetadata(const std::string& name, T value)
        {
            if (!IsMetadataType<T>())
            {
                return false;
            }

            metadata[name] = value;
            return true;
        }

        std::any GetMetadata(const std::string& name) const
        {
            auto iter = metadata.find(name);
            if (iter == metadata.end())
            {
                return {};
            }

            return iter->second;
        }

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
                if (res)
                    res->Set(Status::Failed, "No value named " + name);

                return {};
            }

            auto hash = typeid(T).hash_code();
            if (hash == val.type().hash_code())
            {
                if (res)
                    res->Set(Status::Success, "");

                return std::any_cast<T>(val);
            }

            auto* handler = TSys::TypeRegistry::GetRegistry()->GetTypeHandle(hash);

            if (handler->CanConvertFrom(val.type().hash_code()))
            {
                if (res)
                    res->Set(Status::Success, "");

                return std::any_cast<T>(handler->ConvertFrom(val, val));
            }

            if (res)
                res->Set(Status::Failed, "Invalid metadata type.");

            return {};
        }

        JsonValue Serialize(JsonDoc& doc) const;

        void Deserialize(JsonValue& val);

        static NodeMetadata DeserializeMetadata(JsonValue val);
    };
}

#endif //GEX_METADATAS_H
