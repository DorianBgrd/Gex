#include "rotation.h"

#include <cmath>


double Math::DegToRad(double deg)
{
    return deg * (PI / 180.0);
}


double Math::RadToDeg(double rad)
{
    return rad * (180.0 / PI);
}


Math::Quaternion Math::Quaternion::operator*(
        const Quaternion& other
) const
{
    return Multiplied(*this, other);
}


double Math::EulerRotation::DegreeX() const
{
    return RadToDeg(x);
}


void Math::EulerRotation::SetDegreeX(double angle)
{
    x = DegToRad(angle);
}


double Math::EulerRotation::DegreeY() const
{
    return RadToDeg(y);
}


void Math::EulerRotation::SetDegreeY(double angle)
{
    y = DegToRad(angle);
}


double Math::EulerRotation::DegreeZ() const
{
    return RadToDeg(z);
}


void Math::EulerRotation::SetDegreeZ(double angle)
{
    z = DegToRad(angle);
}


Math::Matrix3 Math::EulerRotation::GetDegrees() const
{
    return {RadToDeg(x), RadToDeg(y), RadToDeg(z)};
}

void Math::EulerRotation::SetDegrees(const Vector3& angles)
{
    SetDegreeX(angles.x);
    SetDegreeY(angles.y);
    SetDegreeZ(angles.z);
}

void Math::EulerRotation::SetDegrees(
        double xAngle, double yAngle, double zAngle
)
{
    SetDegreeX(xAngle);
    SetDegreeY(yAngle);
    SetDegreeZ(zAngle);
}


Math::Quaternion Math::Multiplied(const Quaternion &q1, const Quaternion &q2)
{
    return {q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
            q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
            q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
            q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z};
}


Math::Quaternion Math::ToQuaternionRotation(const EulerRotation &euler)
{
    Math::Quaternion x = {std::sin(euler.x / 2), 0, 0, std::cos(euler.x / 2.0)};
    Math::Quaternion y = {0, std::sin(euler.y / 2), 0, std::cos(euler.y / 2.0)};
    Math::Quaternion z = {0, 0, std::sin(euler.z / 2), std::cos(euler.z / 2)};

    if (euler.order == RotateOrder::XYZ)
    {
        return x * y * z;
    }
    if (euler.order == RotateOrder::XZY)
    {
        return x * z * y;
    }
    if (euler.order == RotateOrder::YXZ)
    {
        return y * x * z;
    }
    if (euler.order == RotateOrder::YZX)
    {
        return y * z * x;
    }
    if (euler.order == RotateOrder::ZXY)
    {
        return z * x * y;
    }
    if (euler.order == RotateOrder::ZYX)
    {
        return z * y * x;
    }

    return {};
}


/*
import numpy as np

def rotmat_to_quat(R):
    tr = np.trace(R)

    if tr > 0:
        S = 2.0 * np.sqrt(tr + 1.0)

        x = (R[2,1] - R[1,2]) / S
        y = (R[0,2] - R[2,0]) / S
        z = (R[1,0] - R[0,1]) / S
        w = 0.25 * S

    elif R[0,0] > R[1,1] and R[0,0] > R[2,2]:
        S = 2.0 * np.sqrt(1.0 + R[0,0] - R[1,1] - R[2,2])

        x = 0.25 * S
        y = (R[0,1] + R[1,0]) / S
        z = (R[0,2] + R[2,0]) / S
        w = (R[2,1] - R[1,2]) / S

    elif R[1,1] > R[2,2]:
        S = 2.0 * np.sqrt(1.0 + R[1,1] - R[0,0] - R[2,2])

        x = (R[0,1] + R[1,0]) / S
        y = 0.25 * S
        z = (R[1,2] + R[2,1]) / S
        w = (R[0,2] - R[2,0]) / S

    else:
        S = 2.0 * np.sqrt(1.0 + R[2,2] - R[0,0] - R[1,1])

        x = (R[0,2] + R[2,0]) / S
        y = (R[1,2] + R[2,1]) / S
        z = 0.25 * S
        w = (R[1,0] - R[0,1]) / S

    return np.array([w, x, y, z])
 */


Math::Quaternion Math::ToQuaternionRotation(const Matrix3& matrix)
{
    double t = matrix.a + matrix.f + matrix.i;
    double s;

    /*
     * [0, 0] a  [0, 1] b  [0, 2] c
     * [1, 0] d  [1, 1] e  [1, 2] f
     * [2, 0] g  [2, 1] h  [2, 2] i
     */

    double v4 = 0.25;

    if (t > 0)
    {
        s = 2.0 * std::sqrt(t + 1.0);

        return {
            (matrix.h - matrix.f) / s,
            (matrix.c - matrix.g) / s,
            (matrix.d - matrix.b) / s,
            v4 * s
        };
    }
    else if (matrix.a > matrix.f && matrix.a > matrix.i)
    {
        s = 2.0 * std::sqrt(1.0 + matrix.a - matrix.e - matrix.f);

        return {
            v4 * s,
            (matrix.b + matrix.d) / s,
            (matrix.c + matrix.g) / s,
            (matrix.h - matrix.f) / s
        };
    }
    else if (matrix.f > matrix.a && matrix.f > matrix.i)
    {
        s = 2.0 * std::sqrt(1.0 + matrix.e - matrix.a - matrix.i);
        return {
                (matrix.b + matrix.d) / s,
                (v4* s),
                (matrix.f + matrix.h) / s,
                (matrix.c - matrix.g) / s
        };
    }
    else
    {
        s = 2.0 * std::sqrt(1.0 + matrix.i - matrix.a - matrix.e);
        return {
                (matrix.c + matrix.g) / s,
                (matrix.f + matrix.h) / s,
                v4 * s,
                (matrix.d - matrix.b) / s,
        };
    }
}


Math::Vector3 Math::ToDegreeRotation(const Vector3& radian)
{
    return {radian.x * PI, radian.y * PI, radian.z  * PI};
}


Math::Matrix3 Math::ToRotationMatrix(const Quaternion &rotation)
{
    double xx = std::pow(rotation.x, 2);
    double yy = std::pow(rotation.y, 2);
    double zz = std::pow(rotation.z, 2);

    double xy = rotation.x * rotation.y;
    double xz = rotation.x * rotation.z;
    double yz = rotation.y * rotation.z;

    double wx = rotation.w * rotation.x;
    double wy = rotation.w * rotation.y;
    double wz = rotation.w * rotation.z;

    return {
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy),
            2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),
            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy)
    };
}


Math::EulerRotation Math::ToEulerRotation(const Matrix3& matrix,
                                          const Math::RotateOrder& order)
{

    double first1, first2;
    double middle;
    double last1, last2;

    // a b c
    // d e f
    // g h i

    double r11 = matrix.a;
    double r12 = matrix.b;
    double r13 = matrix.c;

    double r21 = matrix.d;
    double r22 = matrix.e;
    double r23 = matrix.f;

    double r31 = matrix.g;
    double r32 = matrix.h;
    double r33 = matrix.i;

    switch (order) {
        case RotateOrder::XYZ:
            first1 = -r23;
            first2 = r33; // -r23, r33
            middle = r13; // r13
            last1 = -r12;
            last2 = r11; // -r12, r11
            break;
        case RotateOrder::YZX:
            first1 = -r31;
            first2 = r11;  // -r31, r11
            middle = r21; // r21
            last1 = -r23;
            last2 = r22; // -r23, r22
            break;
        case RotateOrder::ZXY:
            first1 = -r12;
            first2 = r22; // -r12, r22
            middle = r32; // r32
            last1 = -r31;
            last2 = r33;  // -r31, r33
            break;
        case RotateOrder::XZY:
            first1 = r32;
            first2 = r22; // r32, r22
            middle = r12; // r12
            last1 = -r13;
            last2 = r11;  // r13, r11
            break;
        case RotateOrder::YXZ:
            first1 = r13;
            first2 = r33; // r13, r33
            middle = r23; // r23
            last1 = r21;
            last2 = r22;  // r21, r22
            break;
        case RotateOrder::ZYX:
            first1 = r32;
            first2 = r33; // r32, r33
            middle = r31; // r31
            last1 = r21;
            last2 = r11;  // r21, r11
            break;
    }

    return {std::atan2(first1, first2),
            std::asin(middle),
            std::atan2(last1, last2),
            order};
}


Math::EulerRotation Math::ToEulerRotation(const Math::Quaternion& quat,
                                                        const Math::RotateOrder& order)
{
    return ToEulerRotation(ToRotationMatrix(quat), order);
}