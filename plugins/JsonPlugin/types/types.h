#ifndef GEX_JSON_TYPES_H
#define GEX_JSON_TYPES_H

#include <memory>
#include <vector>

#include "Tsys/include/tsys.h"
#include "rapidjson/document.h"

#include "jsonDefs.h"

namespace JsonPlugin
{

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

        std::any FromPython(const boost::python::object&) const override;

        boost::python::object ToPython(const std::any&) const override;

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

        std::any FromPython(const boost::python::object&) const override;

        boost::python::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        std::string ApiName() const override;

        size_t ValueHash(const std::any& val) const override;
    };
}


#endif //GEX_JSON_TYPES_H
