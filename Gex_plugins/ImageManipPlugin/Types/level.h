#ifndef GEX_LEVEL_H
#define GEX_LEVEL_H

#include "Gex_plugins/export.h"
#include "Tsys/tsys.h"


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


    void SerializeLevelMap(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document& document);

    void DeserializeLevelMap(const TSys::TypedValuePtr& v,
                             rapidjson::Value& value,
                             rapidjson::Document&);

}

#endif //GEX_LEVEL_H
