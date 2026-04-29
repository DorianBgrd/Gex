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


    class JsonValueHandler: public TSys::TypeHandler
    {
    public:
        JsonValueHandler();

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any& v1, const std::any& v2) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        std::string ApiName() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    class JsonEditorHandler: public TSys::TypeHandler
    {
    public:
        JsonEditorHandler();

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any& v1, const std::any& v2) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        std::string ApiName() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    class JsonReaderHandler: public TSys::TypeHandler
    {
        JsonReaderHandler();

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any& v1, const std::any& v2) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        std::string ApiName() const override;

        size_t ValueHash(const std::any& val) const override;
    };
}


#endif //GEX_JSON_TYPES_H
