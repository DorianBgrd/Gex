#ifndef GEX_VECTOR_H
#define GEX_VECTOR_H

#include "../export.h"


namespace Math
{
    class Point2;
    class Point3;


    struct Plugin_API Vector2
    {
        double x;
        double y;

        Vector2() = default;

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

}

#endif //GEX_VECTOR_H
