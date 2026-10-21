#ifndef GEX_ROTATION_H
#define GEX_ROTATION_H

#include "vector.h"
#include "matrix.h"

#include "LocalApi.h"

#define PI 3.14159265358979323846

namespace Math
{

    MathPlugin_API
    double DegToRad(double deg);


    MathPlugin_API
    double RadToDeg(double rad);


    struct MathPlugin_API Quaternion
    {
        double x;
        double y;
        double z;
        double w;

        Quaternion operator*(const Quaternion& other) const;
    };


    enum class MathPlugin_API RotateOrder
    {
        XYZ,
        YZX,
        ZXY,
        XZY,
        YXZ,
        ZYX
    };


    struct MathPlugin_API EulerRotation
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

    MathPlugin_API
    Quaternion Multiplied(const Quaternion& q1, const Quaternion& q2);

    MathPlugin_API
    Quaternion ToQuaternionRotation(const EulerRotation& euler);

    MathPlugin_API
    Quaternion ToQuaternionRotation(const Matrix3& euler);

    MathPlugin_API
    Vector3 ToDegreeRotation(const Vector3& radian);

    MathPlugin_API
    Matrix3 ToRotationMatrix(const Quaternion& rotation);

    MathPlugin_API
    EulerRotation ToEulerRotation(const Matrix3& matrix,
                                  const RotateOrder& order);

    MathPlugin_API
    EulerRotation ToEulerRotation(const Quaternion& quat,
                                  const RotateOrder& order);
}

#endif //GEX_ROTATION_H
