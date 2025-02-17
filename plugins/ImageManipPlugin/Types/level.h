#ifndef GEX_LEVEL_H
#define GEX_LEVEL_H

#include "plugins/export.h"
#include "Tsys/include/tsys.h"


namespace ImageManip::Types
{
    class Plugin_API LevelMap
    {
        double low = 0;
        double high = 1;
        double mid = 0.5;

        double clampLow = 0;
        double clampHigh = 1;

    public:
        LevelMap() = default;

        LevelMap(double low, double high, double mid,
                 double clampLow, double clampHigh);

        LevelMap(const LevelMap& other);

        double Low() const;

        void SetLow(double low);

        double High() const;

        void SetHigh(double high);

        double Mid() const;

        void SetMid(double mid);

        double ClampLow() const;

        void SetClampLow(double clamp);

        double ClampHigh() const;

        void SetClampHigh(double clamp);

        bool operator==(const LevelMap& other) const;
    };


    struct LevelMapHandler: public TSys::TypeHandler
    {
        std::any InitValue() const override;

        std::any FromPython(const boost::python::object&) const override;

        boost::python::object ToPython(const std::any&) const override;

        std::any CopyValue(const std::any& source) const override;

        size_t Hash() const override;

        std::string PythonName() const override;

        std::string ApiName() const override;

        size_t ValueHash(const std::any& val) const override;

        bool CompareValue(const std::any& v1, const std::any& v2) const override;

        virtual void SerializeValue(const std::any& v, rapidjson::Value& value,
                                    rapidjson::Document& document) const
                                    override;

        virtual std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override;

        virtual void SerializeConstruction(const std::any& v, rapidjson::Value& value,
                                           rapidjson::Document& document)
                                           const override;

        virtual std::any DeserializeConstruction(rapidjson::Value& value) const override;

    };

}

#endif //GEX_LEVEL_H
