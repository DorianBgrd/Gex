#include "matrix.h"
#include "vector.h"


double Math::GetDeterminant(
        const Math::Matrix3& matrix
)
{
    double f = matrix.a;
    double g = matrix.b;
    double h = matrix.c;
    double j = matrix.d;
    double k = matrix.e;
    double l = matrix.f;
    double n = matrix.g;
    double o = matrix.h;
    double p = matrix.i;

    return f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n);
}


Math::Matrix3 Math::Transposed(const Matrix3& matrix)
{
    return {
        matrix.a,matrix.d, matrix.g,
        matrix.b, matrix.e, matrix.h,
        matrix.c, matrix.f, matrix.i
    };
}


Math::Matrix3 Math::ToMatrix(const Vector3& x,
                             const Vector3& y,
                             const Vector3& z)
{
    return {
        x.x, x.y, x.z,
        y.x, y.y, y.z,
        z.x, z.y, z.z
    };
}


Math::Matrix4
Math::Matrix4::operator*(const Matrix4& other) const
{
    return Multiplied(*this, other);
}


Math::Matrix4
Math::Matrix4::operator+(const Matrix4& other) const
{
    return Added(*this, other);
}


Math::Matrix4 Math::GetIdentity()
{
    return {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}


void Math::GetArray(const Matrix4& matrix, double values[4][4])
{
    values[0][0] = matrix.a;
    values[0][1] = matrix.b;
    values[0][2] = matrix.c;
    values[0][3] = matrix.d;

    values[1][0] = matrix.e;
    values[1][1] = matrix.f;
    values[1][2] = matrix.g;
    values[1][3] = matrix.h;

    values[2][0] = matrix.i;
    values[2][1] = matrix.j;
    values[2][2] = matrix.k;
    values[2][3] = matrix.l;

    values[3][0] = matrix.m;
    values[3][1] = matrix.n;
    values[3][2] = matrix.o;
    values[3][3] = matrix.p;
}



Math::Matrix4 Math::GetInverse(const Matrix4& matrix)
{
    double a = matrix.a;
    double b = matrix.b;
    double c = matrix.c;
    double d = matrix.d;
    double e = matrix.e;
    double f = matrix.f;
    double g = matrix.g;
    double h = matrix.h;
    double i = matrix.i;
    double j = matrix.j;
    double k = matrix.k;
    double l = matrix.l;
    double m = matrix.m;
    double n = matrix.n;
    double o = matrix.o;
    double p = matrix.p;


    Matrix3 C11 =  {
            f, g, h,
            j, k, l,
            n, o, p
    };

    Matrix3 C12 = {
            e, g, h,
            i, k, l,
            m, o, p
    };

    Matrix3 C13 = {
            e, f, h,
            i, j, l,
            m, n, p
    };

    Matrix3 C14 = {
            e, f, g,
            i, j, k,
            m, n, o
    };

    Matrix3 C21 = {
            b, c, d,
            j, k, l,
            n, o, p
    };

    Matrix3 C22 = {
            a, c, d,
            i, k, l,
            m, o, p
    };

    Matrix3 C23 = {
            a, b, d,
            i, j, l,
            m, n, p
    };

    Matrix3 C24 = {
            a, b, c,
            i, j, k,
            m, n, o
    };

    Matrix3 C31 = {
            b, c, d,
            f, g, h,
            n, o, p
    };

    Matrix3 C32 = {
            a, c, d,
            e, g, h,
            m, o, p
    };

    Matrix3 C33 = {
            a, b, d,
            e, f, h,
            m, n, p
    };

    Matrix3 C34 = {
            a, b, c,
            e, f, g,
            m, n, o
    };

    Matrix3 C41 = {
            b, c, d,
            f, g, h,
            j, k, l
    };

    Matrix3 C42 = {
            a, c, d,
            e, g, h,
            i, k, l
    };

    Matrix3 C43 = {
            a, b, d,
            e, f, h,
            i, j, l
    };

    Matrix3 C44 = {
            a, b, c,
            e, f, g,
            i, j, k
    };

    Matrix4 cof = {
            GetDeterminant(C11), -GetDeterminant(C21), GetDeterminant(C31), -GetDeterminant(C41),
            -GetDeterminant(C12), GetDeterminant(C22), -GetDeterminant(C32), GetDeterminant(C42),
            GetDeterminant(C13), -GetDeterminant(C23), GetDeterminant(C33), -GetDeterminant(C43),
            -GetDeterminant(C14), GetDeterminant(C24), -GetDeterminant(C34), GetDeterminant(C44),
    };

    double detA = GetDeterminant(matrix);

    double factor = 1.0 / detA;

    Matrix4 inverted = {
            factor * cof.a, factor * cof.b, factor * cof.c, factor * cof.d,
            factor * cof.e, factor * cof.f, factor * cof.g, factor * cof.h,
            factor * cof.i, factor * cof.j, factor * cof.k, factor * cof.l,
            factor * cof.m, factor * cof.n, factor * cof.o, factor * cof.p
    };

    return inverted;
}


Math::Matrix4 Math::Transposed(const Matrix4& matrix)
{
    return {
        matrix.a, matrix.e, matrix.i,
        matrix.b, matrix.f, matrix.j,
        matrix.c, matrix.g, matrix.k,
        matrix.d, matrix.h, matrix.l
    };
}


double MultiplyRowCol(const double matrix1[4][4], unsigned int r,
                      const double matrix2[4][4], unsigned int c)
{
    return (matrix1[r][0] * matrix2[0][c] + matrix1[r][1] * matrix2[1][c] +
            matrix1[r][2] * matrix2[2][c] + matrix1[r][3] * matrix2[3][c]);
}


Math::Matrix4 Math::Multiplied(
        const Matrix4& matrix1, const Matrix4& matrix2
)
{
    double array1[4][4];
    double array2[4][4];

    GetArray(matrix1, array1);
    GetArray(matrix2, array2);

    return {
            MultiplyRowCol(array1, 0, array2, 0),
            MultiplyRowCol(array1, 0, array2, 1),
            MultiplyRowCol(array1, 0, array2, 2),
            MultiplyRowCol(array1, 0, array2, 3),

            MultiplyRowCol(array1, 1, array2, 0),
            MultiplyRowCol(array1, 1, array2, 1),
            MultiplyRowCol(array1, 1, array2, 2),
            MultiplyRowCol(array1, 1, array2, 3),

            MultiplyRowCol(array1, 2, array2, 0),
            MultiplyRowCol(array1, 2, array2, 1),
            MultiplyRowCol(array1, 2, array2, 2),
            MultiplyRowCol(array1, 2, array2, 3),

            MultiplyRowCol(array1, 3, array2, 0),
            MultiplyRowCol(array1, 3, array2, 1),
            MultiplyRowCol(array1, 3, array2, 2),
            MultiplyRowCol(array1, 3, array2, 3),
    };
}


Math::Matrix4 Math::Added(
        const Matrix4& matrix1,
        const Matrix4& matrix2
)
{
    return {
        matrix1.a + matrix2.a, matrix1.b + matrix2.b, matrix1.c + matrix2.c, matrix1.d + matrix2.d,
        matrix1.e + matrix2.e, matrix1.f + matrix2.f, matrix1.g + matrix2.g, matrix1.h + matrix2.h,
        matrix1.i + matrix2.i, matrix1.j + matrix2.j, matrix1.k + matrix2.k, matrix1.l + matrix2.l,
        matrix1.m + matrix2.m, matrix1.n + matrix2.n, matrix1.o + matrix2.o, matrix1.p + matrix2.p
    };
}



double Math::GetDeterminant(const Matrix4& matrix)
{
    Matrix3 m1 = {
            matrix.f, matrix.g, matrix.h,
            matrix.j, matrix.k, matrix.l,
            matrix.n, matrix.o, matrix.p
    };

    Matrix3 m2 = {
            matrix.e, matrix.g, matrix.h,
            matrix.i, matrix.k, matrix.l,
            matrix.m, matrix.o, matrix.p
    };

    Matrix3 m3 = {
            matrix.e, matrix.f, matrix.h,
            matrix.i, matrix.j, matrix.l,
            matrix.m, matrix.n, matrix.p
    };

    Matrix3 m4 = {
            matrix.e, matrix.f, matrix.g,
            matrix.i, matrix.j, matrix.k,
            matrix.m, matrix.n, matrix.o
    };

    return (
            matrix.a * GetDeterminant(m1) +
            matrix.b * GetDeterminant(m2) +
            matrix.c * GetDeterminant(m3) +
            matrix.d * GetDeterminant(m4)
    );
}
