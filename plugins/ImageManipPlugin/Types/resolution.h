#ifndef GEX_IMP_RESOLUTION_H
#define GEX_IMP_RESOLUTION_H

#include "Tsys/include/tsys.h"

#include "../export.h"

#include <QColor>
#include <array>

namespace ImageManip::Types
{
    typedef std::array<int, 2> Resolution;

    struct Plugin_API ResolutionHandler: TSys::TypeHandler
    {
        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            Resolution res = std::any_cast<Resolution>(v);

            value.PushBack(rapidjson::Value().SetInt(res.at(0)),
                           document.GetAllocator());
            value.PushBack(rapidjson::Value().SetInt(res.at(1)),
                           document.GetAllocator());
        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            Resolution res = {
                    value[0].GetInt(),
                    value[1].GetInt()
            };

            return std::make_any<Resolution>(res);
        }

        void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                   rapidjson::Document& document)
        const override
        {

        }

        std::any DeserializeConstruction(rapidjson::Value& value) const override
        {
            return InitValue();
        }

        std::any InitValue() const override
        {
            return std::make_any<Resolution>(Resolution());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return std::any_cast<Resolution>(v1) == std::any_cast<Resolution>(v2);
        }

        std::any FromPython(const boost::python::object&) const override
        {
            return InitValue();
        }

        boost::python::object ToPython(const std::any&) const override
        {
            return {};
        }

        std::any CopyValue(const std::any& source) const override
        {
            return std::make_any<Resolution>(Resolution(std::any_cast<Resolution>(source)));
        }

        size_t Hash() const override
        {
            return typeid(Resolution).hash_code();
        }

        std::string PythonName() const override
        {
            return "Resolution";
        }

        std::string ApiName() const override
        {
            return "Resolution";
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };
}

#endif //GEX_IMP_RESOLUTION_H
