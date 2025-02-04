#ifndef GEX_IMAGE_H
#define GEX_IMAGE_H

#include "Tsys/include/tsys.h"
#include <QPixmap>

#include "../export.h"


namespace ImageManip::Types
{
    struct Plugin_API ImageHandler: TSys::TypeHandler
    {
        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override {}

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            return InitValue();
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
            return std::make_any<QImage>(QImage());
        }

        bool CompareValue(const std::any& v1, const std::any& v2) const override
        {
            return false;
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
            return std::make_any<QImage>(QImage(std::any_cast<QImage>(source)));
        }

        size_t Hash() const override
        {
            return typeid(QImage).hash_code();
        }

        std::string PythonName() const override
        {
            return "QImage";
        }

        std::string ApiName() const override
        {
            return "QImage";
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };
}


#endif //GEX_IMAGE_H
