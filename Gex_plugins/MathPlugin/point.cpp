#include "point.h"
#include "vector.h"

#include <cmath>


bool Math::Point2::operator==(const Point2& other) const
{
    return (x == other.x && y == other.y);
}

double Math::Point2::Distance(const Point2& other) const
{
    return Distance(*this, other);
}

double Math::Point2::Distance(
        const Point2& point1,
        const Point2& point2)
{
    return std::sqrt(
            std::pow(point1.x - point2.x, 2) +
            std::pow(point1.y - point2.y, 2)
    );
}

Math::Point2 Math::Point2::FromVector2(const Vector2& other)
{
    return {other.x, other.y};
}



Math::Point3::Point3(
        double x_,
        double y_,
        double z_
)
{
    x = x_;
    y = y_;
    z = z_;
}


Math::Point3::Point3(
        const Point3& other
)
{
    x = other.x;
    y = other.y;
    z = other.z;
}


bool Math::Point3::operator==(const Point3& other) const
{
    return (x == other.x && y == other.y && z == other.z);
}


Math::Point3 Math::Point3::operator *(double value) const
{
    return {x * value, y * value, z * value};
}


Math::Point3 Math::Point3::operator +(const Point3& other) const
{
    return {x + other.x, y + other.y, z + other.z};
}


double Math::Point3::Distance(const Point3& other) const
{
    return Distance(*this, other);
}

double Math::Point3::Distance(
        const Point3& point1,
        const Point3& point2)
{
    return std::sqrt(
            std::pow(point1.x - point2.x, 2) +
            std::pow(point1.y - point2.y, 2) +
            std::pow(point1.z - point2.z, 2)
    );
}


Math::Point3 Math::Point3::FromVector3(const Vector3& other)
{
    return {other.x, other.y, other.z};
}


void Math::Point3::Swap(const Point3& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}
