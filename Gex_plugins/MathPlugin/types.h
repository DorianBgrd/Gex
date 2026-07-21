#ifndef GEX_TYPES_H
#define GEX_TYPES_H

#include "../export.h"
#include "LocalApi.h"
#include "Tsys/tsys.h"


namespace Math::Types
{
    class Vector2;
    class Vector3;



    class Point2Handle: public TSys::TypeHandler
    {
    public:
        Point2Handle();

        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    class Vector2Handle: public TSys::TypeHandler
    {
    public:
        Vector2Handle();

        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    class Point3Handle: public TSys::TypeHandler
    {
    public:
        Point3Handle();

        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };


    class Vector3Handle: public TSys::TypeHandler
    {
    public:
        Vector3Handle();

        std::string ApiName() const override;

        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document) const override;

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document) const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(const std::any&, const std::any&) const override;

        std::any FromPython(const pybind11::object&) const override;

        pybind11::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        size_t ValueHash(const std::any& val) const override;
    };
}

#endif //GEX_TYPES_H
