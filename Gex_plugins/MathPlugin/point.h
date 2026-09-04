#ifndef GEX_POINT_H
#define GEX_POINT_H

#include "../export.h"


namespace Math
{
    class Vector2;
    class Vector3;


    struct Plugin_API Point2
    {
        double x;
        double y;

        Point2() = default;

        bool operator ==(const Point2& other) const;

        double Distance(const Point2& other) const;

        static double Distance(const Point2& point1,
                               const Point2& point2);


        static Point2 FromVector2(const Vector2& other);
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

}

#endif //GEX_POINT_H
