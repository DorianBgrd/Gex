#ifndef GEX_ROTATION_H
#define GEX_ROTATION_H

#include "vector.h"
#include "matrix.h"

#define PI 3.14159265358979323846

namespace Math
{

    double DegToRad(double deg);

    double RadToDeg(double rad);

    struct Quaternion
    {
        double x;
        double y;
        double z;
        double w;

        Quaternion operator*(const Quaternion& other) const;
    };

    enum class RotateOrder
    {
        XYZ,
        YZX,
        ZXY,
        XZY,
        YXZ,
        ZYX
    };

    struct EulerRotation
    {
        double x;
        double y;
        double z;

        RotateOrder order = RotateOrder::XYZ;

        double DegreeX() const;

        void SetDegreeX(double angle);

        double DegreeY() const;

        void SetDegreeY(double angle);

        double DegreeZ() const;

        void SetDegreeZ(double angle);

        Matrix3 GetDegrees() const;

        void SetDegrees(const Vector3& angles);

        void SetDegrees(double xAngle,
                        double yAngle,
                        double zAngle);
    };

    Quaternion Multiplied(const Quaternion& q1, const Quaternion& q2);

    Quaternion ToQuaternionRotation(const EulerRotation& euler);

    Quaternion ToQuaternionRotation(const Matrix3& euler);

    Vector3 ToDegreeRotation(const Vector3& radian);

    Matrix3 ToRotationMatrix(const Quaternion& rotation);

    EulerRotation ToEulerRotation(const Matrix3& matrix,
                                  const RotateOrder& order);

    EulerRotation ToEulerRotation(const Quaternion& quat,
                                  const RotateOrder& order);
}

#endif //GEX_ROTATION_H
