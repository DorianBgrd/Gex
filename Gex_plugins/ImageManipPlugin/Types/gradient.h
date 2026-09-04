#ifndef GEX_GRADIENT_H
#define GEX_GRADIENT_H

#include <map>
#include <QColor>

#include "Tsys/tsys.h"


#include "../export.h"


namespace ImageManip::Types
{

    class Plugin_API LinearGradient
    {
        std::map<double, QColor> steps;

    public:
        LinearGradient();

        LinearGradient(std::map<double, QColor> steps);

        void AddStep(double x, const QColor& color);

        void InsertStep(double x);

        bool RemoveStep(double x);

        bool MoveStep(double x, double dx);

        bool EditStep(double x, QColor color);

        QColor ColorAtStep(double x) const;

        std::vector<double> Steps() const;

        std::map<double, QColor> StepColors() const;

        bool operator==(const LinearGradient& other)
        {
            return other.StepColors() == StepColors();
        }
    };


    void SerializeGradient(const TSys::TypedValuePtr& v,
                           rapidjson::Value& value,
                           rapidjson::Document& document);


    void DeserializeGradient(const TSys::TypedValuePtr& v,
                             rapidjson::Value& value,
                             rapidjson::Document&);

}

#endif //GEX_GRADIENT_H
