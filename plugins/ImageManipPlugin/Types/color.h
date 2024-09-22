#ifndef GEX_IMP_COLOR_H
#define GEX_IMP_COLOR_H

#include "Tsys/include/tsys.h"

#include "../export.h"

#include <QColor>

namespace ImageManip::Types
{
    struct Plugin_API ColorHandler: TSys::TypeHandler
    {
        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
                            const override
        {
            QColor color = std::any_cast<QColor>(v);

            value.PushBack(rapidjson::Value().SetInt(color.red()),
                           document.GetAllocator());
            value.PushBack(rapidjson::Value().SetInt(color.green()),
                           document.GetAllocator());
            value.PushBack(rapidjson::Value().SetInt(color.blue()),
                           document.GetAllocator());
            value.PushBack(rapidjson::Value().SetInt(color.alpha()),
                           document.GetAllocator());
        }

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            QColor color(
                    value[0].GetInt(),
                    value[1].GetInt(),
                    value[2].GetInt(),
                    value[3].GetInt()
                    );

            return std::make_any<QColor>(color);
        }

        void SerializeConstruction(std::any v, rapidjson::Value& value,
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
            return std::make_any<QColor>(QColor());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return std::any_cast<QColor>(v1) == std::any_cast<QColor>(v2);
        }

        std::any FromPython(boost::python::object) const override
        {
            return InitValue();
        }

        boost::python::object ToPython(std::any) const override
        {
            return {};
        }

        std::any CopyValue(std::any source) const override
        {
            return std::make_any<QColor>(QColor(std::any_cast<QColor>(source)));
        }

        size_t Hash() const override
        {
            return typeid(QColor).hash_code();
        }

        std::string Name() const override
        {
            return typeid(QColor).name();
        }

        std::string PythonName() const override
        {
            return "QColor";
        }

        std::string ApiName() const override
        {
            return "QColor";
        }

        size_t ValueHash(std::any val) const override
        {
            return 0;
        }
    };
}

#endif //GEX_IMP_COLOR_H