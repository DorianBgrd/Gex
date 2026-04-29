#ifndef GEX_TYPES_H
#define GEX_TYPES_H

#include "../export.h"
#include "LocalApi.h"
#include "Tsys/tsys.h"


namespace Math::Types
{
    class Vector2;
    class Vector3;


    struct Plugin_API Point2
    {
        double x;
        double y;

        bool operator ==(const Point2& other) const;

        double Distance(const Point2& other) const;

        static double Distance(const Point2& point1,
                               const Point2& point2);


        static Point2 FromVector2(const Vector2& other);
    };


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



    struct Plugin_API Vector2
    {
        double x;
        double y;

        bool operator ==(const Vector2& other) const;

        double Magnitude() const;

        void Normalize();

        Vector2 Normalized() const;

        void Multiply(double mult);

        Vector2 Multiplied(double mult) const;

        void Divide(double div);

        Vector2 Divided(double div) const;

        void Add(const Vector2& other);

        Vector2 Added(const Vector2& other) const;

        void Subtract(const Vector2& other);

        Vector2 Subtracted(const Vector2& other) const;

        double DotProduct(const Vector2& other) const;

        static double DotProduct(const Vector2& vector1,
                                 const Vector2& vector2);

        double Angle(const Vector2& other) const;

        static double Angle(const Vector2& vector1,
                            const Vector2& vector2);

        static Vector2 FromPoints(const Point2& start, const Point2& end);

//        static void RegisterPythonWrapper();

        static Vector2 FromPoint2(const Point2& other);

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


    struct Plugin_API Point3
    {
        double x = 0;
        double y = 0;
        double z = 0;

        Point3() = default;

        Point3(double x, double y, double z);

        Point3(const Point3& other);

        bool operator ==(const Point3& other) const;

        Point3 operator *(double value) const;

        Point3 operator +(const Point3& other) const;

        double Distance(const Point3& other) const;

        static double Distance(const Point3& point1,
                               const Point3& point2);

        static Point3 FromVector3(const Vector3& other);

        void Swap(const Point3& other);
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


    struct Plugin_API Vector3
    {
        double x = 0;
        double y = 0;
        double z = 0;

        Vector3() = default;

        Vector3(double x, double y, double z);

        Vector3(const Vector3& other);

        bool operator ==(const Vector3& other) const;

        void operator +=(const Vector3& other);

        double Magnitude() const;

        void Normalize();

        Vector3 Normalized() const;

        void Multiply(double mult);

        Vector3 Multiplied(double mult) const;

        void Divide(double div);

        Vector3 Divided(double div) const;

        void Add(const Vector3& other);

        Vector3 Added(const Vector3& other) const;

        void Subtract(const Vector3& other);

        Vector3 Subtracted(const Vector3& other) const;

        double DotProduct(const Vector3& other) const;

        static double DotProduct(const Vector3& vector1,
                                 const Vector3& vector2);

        double Angle(const Vector3& other) const;

        static double Angle(const Vector3& vector1,
                            const Vector3& vector2);

        Vector3 CrossProduct(const Vector3& other) const;

        static Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);

        static Vector3 FromPoints(const Point3& start, const Point3& end);

        static Vector3 FromPoint3(const Point3& other);
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
