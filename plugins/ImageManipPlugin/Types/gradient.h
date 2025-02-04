#ifndef GEX_GRADIENT_H
#define GEX_GRADIENT_H

#include <map>
#include <QColor>

#include "Tsys/include/tsys.h"


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


    struct Plugin_API LinearGradientHandler: TSys::TypeHandler
    {
        void SerializeValue(const std::any& v, rapidjson::Value& value,
                            rapidjson::Document& document)
        const override
        {
            auto gradient = std::any_cast<LinearGradient>(v);
            for (auto step : gradient.StepColors())
            {
                rapidjson::Value& stepValue = rapidjson::Value().SetArray();

                rapidjson::Value stepX = rapidjson::Value(rapidjson::kNumberType);
                stepX.SetDouble(step.first);
                stepValue.PushBack(stepX.Move(), document.GetAllocator());

                rapidjson::Value& color = rapidjson::Value().SetArray();

                rapidjson::Value cR = rapidjson::Value(rapidjson::kNumberType);
                cR.SetInt64(step.second.red());
                color.PushBack(cR.Move(), document.GetAllocator());

                rapidjson::Value cG = rapidjson::Value(rapidjson::kNumberType);
                cG.SetInt64(step.second.green());
                color.PushBack(cG.Move(), document.GetAllocator());

                rapidjson::Value cB = rapidjson::Value(rapidjson::kNumberType);
                cB.SetInt64(step.second.blue());
                color.PushBack(cB.Move(), document.GetAllocator());

                stepValue.PushBack(color.Move(), document.GetAllocator());

                value.PushBack(stepValue.Move(), document.GetAllocator());
            }
        }

        std::any DeserializeValue(const std::any& v, rapidjson::Value& value) const override
        {
            LinearGradient gradient;

            rapidjson::Value& steps = value.GetArray();
            for (unsigned int i = 0; i < steps.Size(); i++)
            {
                rapidjson::Value& step = steps[i];

                double x = step[0].GetDouble();
                QColor color(step[1][0].GetInt64(),
                             step[1][1].GetInt64(),
                             step[1][2].GetInt64());

                gradient.AddStep(x, color);
            }

            return std::make_any<LinearGradient>(gradient);
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
            return std::make_any<LinearGradient>(LinearGradient());
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
            return std::make_any<LinearGradient>(LinearGradient(
                    std::any_cast<LinearGradient>(source)));
        }

        size_t Hash() const override
        {
            return typeid(LinearGradient).hash_code();
        }

        std::string PythonName() const override
        {
            return "LinearGradient";
        }

        std::string ApiName() const override
        {
            return "LinearGradient";
        }

        size_t ValueHash(const std::any& val) const override
        {
            return 0;
        }
    };
}

#endif //GEX_GRADIENT_H
