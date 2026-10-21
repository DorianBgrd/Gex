#ifndef GEX_MATRIX_H
#define GEX_MATRIX_H


#include "LocalApi.h"


namespace Math
{
    class Vector3;


    struct MathPlugin_API Matrix3
    {
        double a, b, c;
        double d, e, f;
        double g, h, i;
    };

    MathPlugin_API
    double GetDeterminant(const Matrix3& matrix);

    MathPlugin_API
    Matrix3 Transposed(const Matrix3& matrix);

    MathPlugin_API
    Matrix3 ToMatrix(const Vector3& x,
                     const Vector3& y,
                     const Vector3& z);

    struct MathPlugin_API Matrix4
    {
        double a, b, c, d;
        double e, f, g, h;
        double i, j, k, l;
        double m, n, o, p;

        Matrix4 operator*(const Matrix4& other) const;

        Matrix4 operator+(const Matrix4& other) const;
    };

    MathPlugin_API
    Matrix4 GetIdentity();

    MathPlugin_API
    void GetArray(const Matrix4& matrix, double values[4][4]);

    MathPlugin_API
    Matrix4 GetInverse(const Matrix4& matrix);

    MathPlugin_API
    Matrix4 Transposed(const Matrix4& matrix);

    MathPlugin_API
    Matrix4 Multiplied(const Matrix4& matrix1,
                       const Matrix4& matrix2);

    MathPlugin_API
    Matrix4 Added(const Matrix4& matrix1,
                  const Matrix4& matrix2);

    MathPlugin_API
    double GetDeterminant(const Matrix4& matrix);

}

#endif //GEX_MATRIX_H
