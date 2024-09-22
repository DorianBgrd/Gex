#ifndef GEX_IMAGE_H
#define GEX_IMAGE_H

#include "Tsys/include/tsys.h"
#include <QPixmap>

#include "../export.h"


namespace ImageManip::Types
{
    struct Plugin_API ImageHandler: TSys::TypeHandler
    {
        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override {}

        std::any DeserializeValue(std::any v, rapidjson::Value& value) const override
        {
            return InitValue();
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
            return std::make_any<QImage>(QImage());
        }

        bool CompareValue(std::any v1, std::any v2) const override
        {
            return false;
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
            return std::make_any<QImage>(QImage(std::any_cast<QImage>(source)));
        }

        size_t Hash() const override
        {
            return typeid(QImage).hash_code();
        }

        std::string Name() const override
        {
            return typeid(QImage).name();
        }

        std::string PythonName() const override
        {
            return "QImage";
        }

        std::string ApiName() const override
        {
            return "QImage";
        }

        size_t ValueHash(std::any val) const override
        {
            return 0;
        }
    };
}


#endif //GEX_IMAGE_H
