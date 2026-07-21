#include "vector.h"
#include "point.h"

#include <cmath>


bool Math::Vector2::operator ==(const Vector2& other) const
{
    return (x == other.x && y == other.y);
}

double Math::Vector2::Magnitude() const
{
    return std::sqrt(x * x + y * y);
}

void Math::Vector2::Normalize()
{
    double mag = Magnitude();

    x = x / mag;
    y = y / mag;
}

Math::Vector2 Math::Vector2::Normalized() const
{
    auto copy = Vector2(*this);

    copy.Normalize();

    return copy;
}

void Math::Vector2::Multiply(double mult)
{
    x = x * mult;
    y = y * mult;
}

Math::Vector2 Math::Vector2::Multiplied(double mult) const
{
    auto copy = Vector2(*this);

    copy.Multiply(mult);

    return copy;
}

void Math::Vector2::Divide(double div)
{
    x = x / div;
    y = y / div;
}

Math::Vector2 Math::Vector2::Divided(double div) const
{
    auto copy = Vector2(*this);

    copy.Divide(div);

    return copy;
}

void Math::Vector2::Add(const Vector2& other)
{
    x = x + other.x;
    y = y + other.y;
}

Math::Vector2 Math::Vector2::Added(const Vector2& other) const
{
    auto copy = Vector2(*this);

    copy.Add(other);

    return copy;
}

void Math::Vector2::Subtract(const Vector2& other)
{
    x = x - other.x;
    y = y - other.y;
}

Math::Vector2 Math::Vector2::Subtracted(const Vector2& other) const
{
    auto copy = Vector2(*this);

    copy.Subtract(other);

    return copy;
}

double Math::Vector2::DotProduct(const Vector2& other) const
{
    return x * other.x + y * other.y;
}

double Math::Vector2::DotProduct(
        const Vector2& vector1,
        const Vector2& vector2
)
{
    return vector1.DotProduct(vector2);
}

double Math::Vector2::Angle(const Vector2& other) const
{
    double dot = DotProduct(other);
    double mag1 = Magnitude();
    double mag2 = other.Magnitude();

    return std::acos(dot / (mag1 * mag2));
}

double Math::Vector2::Angle(
        const Vector2& vector1,
        const Vector2& vector2
)
{
    return vector1.Angle(vector2);
}


Math::Vector2 Math::Vector2::FromPoints(
        const Point2& start, const Point2& end
)
{
    return {end.x - start.x, end.y - start.y};
}


Math::Vector2 Math::Vector2::FromPoint2(const Point2& other)
{
    return {other.x, other.y};
}


Math::Vector3::Vector3(
        double x_,
        double y_,
        double z_
)
{
    x = x_;
    y = y_;
    z = z_;
}


Math::Vector3::Vector3(
        const Vector3& other
)
{
    x = other.x;
    y = other.y;
    z = other.z;
}


bool Math::Vector3::operator ==(const Vector3& other) const
{
    return (x == other.x && y == other.y && z == other.z);
}

void Math::Vector3::operator +=(const Vector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

double Math::Vector3::Magnitude() const
{
    return std::sqrt(x * x + y * y + z * z);
}

void Math::Vector3::Normalize()
{
    double mag = Magnitude();

    x = x / mag;
    y = y / mag;
    z = z / mag;
}

Math::Vector3 Math::Vector3::Normalized() const
{
    auto copy = Vector3(*this);

    copy.Normalize();

    return copy;
}

void Math::Vector3::Multiply(double mult)
{
    x = x * mult;
    y = y * mult;
    z = z * mult;
}

Math::Vector3 Math::Vector3::Multiplied(double mult) const
{
    auto copy = Vector3(*this);

    copy.Multiply(mult);

    return copy;
}

void Math::Vector3::Divide(double div)
{
    x = x / div;
    y = y / div;
    z = z / div;
}

Math::Vector3 Math::Vector3::Divided(double div) const
{
    auto copy = Vector3(*this);

    copy.Divide(div);

    return copy;
}

void Math::Vector3::Add(const Vector3& other)
{
    x = x + other.x;
    y = y + other.y;
    z = z + other.z;
}

Math::Vector3 Math::Vector3::Added(const Vector3& other) const
{
    auto copy = Vector3(*this);

    copy.Add(other);

    return copy;
}

void Math::Vector3::Subtract(const Vector3& other)
{
    x = x - other.x;
    y = y - other.y;
    z = z - other.z;
}

Math::Vector3 Math::Vector3::Subtracted(const Vector3& other) const
{
    auto copy = Vector3(*this);

    copy.Subtract(other);

    return copy;
}

double Math::Vector3::DotProduct(const Vector3& other) const
{
    return x * other.x + y * other.y + z * other.z;
}

double Math::Vector3::DotProduct(
        const Vector3& vector1,
        const Vector3& vector2
)
{
    return vector1.DotProduct(vector2);
}

double Math::Vector3::Angle(const Vector3& other) const
{
    double dot = DotProduct(other);
    double mag1 = Magnitude();
    double mag2 = other.Magnitude();

    return std::acos(dot / (mag1 * mag2));
}

double Math::Vector3::Angle(
        const Vector3& vector1,
        const Vector3& vector2
)
{
    return vector1.Angle(vector2);
}


Math::Vector3 Math::Vector3::CrossProduct(
        const Vector3& other
) const
{
    return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    };
}


Math::Vector3 Math::Vector3::CrossProduct(
        const Vector3& v1, const Vector3& v2
)
{
    return v1.CrossProduct(v2);
}


Math::Vector3 Math::Vector3::FromPoints(
        const Point3& start, const Point3& end
)
{
    return {
            end.x - start.x, end.y - start.y, end.z - start.z
    };
}


Math::Vector3 Math::Vector3::FromPoint3(const Point3 &other)
{
    return {other.x, other.y, other.z};
}

