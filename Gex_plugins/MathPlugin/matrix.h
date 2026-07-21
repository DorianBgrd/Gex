#ifndef GEX_MATRIX_H
#define GEX_MATRIX_H

namespace Math
{
    class Vector3;

    struct Matrix3
    {
        double a, b, c;
        double d, e, f;
        double g, h, i;
    };

    double GetDeterminant(const Matrix3& matrix);

    Matrix3 Transposed(const Matrix3& matrix);

    Matrix3 ToMatrix(const Vector3& x,
                     const Vector3& y,
                     const Vector3& z);

    struct Matrix4
    {
        double a, b, c, d;
        double e, f, g, h;
        double i, j, k, l;
        double m, n, o, p;

        Matrix4 operator*(const Matrix4& other) const;

        Matrix4 operator+(const Matrix4& other) const;
    };

    Matrix4 GetIdentity();

    void GetArray(const Matrix4& matrix, double values[4][4]);

    Matrix4 GetInverse(const Matrix4& matrix);

    Matrix4 Transposed(const Matrix4& matrix);

    Matrix4 Multiplied(const Matrix4& matrix1,
                       const Matrix4& matrix2);

    Matrix4 Added(const Matrix4& matrix1,
                  const Matrix4& matrix2);

    double GetDeterminant(const Matrix4& matrix);

}

#endif //GEX_MATRIX_H
