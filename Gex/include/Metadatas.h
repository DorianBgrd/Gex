#ifndef GEX_METADATAS_H
#define GEX_METADATAS_H

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
#define IsAvailableType(T) bool(TSys::TypeRegistry::GetRegistry()->IsRegistered(T))

namespace Gex
{
    typedef rapidjson::Value& JsonValue;
    typedef rapidjson::Document& JsonDoc;
    typedef std::any Val;
    typedef size_t Hash;

    struct Metadata
    {
        size_t hash;
        Val value;

    public:
        Metadata(size_t h);

        template<class T>
        Metadata(): Metadata(typeid(T).hash_code()){}


        bool ValueSet(std::any val);

        template<class T>
        bool Set(T val)
        {
            return ValueSet(std::any_cast<T>(val));
        }

        std::any ValueGet(const size_t& hash);

        template<class T>
        bool Get(T& val)
        {
            std::any cv = ValueGet(typeid(val).hash_code());
            if (!cv.has_value())
                return false;

            val = std::make_any<T>(cv);
            return true;
        }

        bool Serialize(JsonValue json, JsonDoc doc);

        bool Deserialize(JsonValue json);
    };
}

#endif //GEX_METADATAS_H
