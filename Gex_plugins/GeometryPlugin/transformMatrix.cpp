#include "transformMatrix.h"


Math::TransformMatrix::TransformMatrix(const Matrix4& mat)
{
    translate = {mat.m, mat.n, mat.o};

    Vector3 xVec = {mat.a, mat.b, mat.c};
    Vector3 yVec = {mat.e, mat.f, mat.g};
    Vector3 zVec = {mat.i, mat.j, mat.k};

    rotate = ToQuaternionRotation(
            ToMatrix(
                    xVec.Normalized(),
                    yVec.Normalized(),
                    zVec.Normalized()
            )
    );

    scale = {
            xVec.Magnitude(),
            yVec.Magnitude(),
            zVec.Magnitude()
    };
}


Math::Vector3 Math::TransformMatrix::GetTranslation() const
{
    return translate;
}


void Math::TransformMatrix::SetTranslation(const Vector3& tr)
{
    translate = tr;
}


Math::Vector3 Math::TransformMatrix::GetScale() const
{
    return scale;
}


void Math::TransformMatrix::SetScale(const Vector3& sc)
{
    scale = sc;
}


Math::Quaternion Math::TransformMatrix::GetRotation() const
{
    return rotate;
}


void Math::TransformMatrix::SetRotation(const Quaternion& quat)
{
    rotate = quat;
}


Math::EulerRotation Math::TransformMatrix::GetEulerRotation(const RotateOrder& order) const
{
    return ToEulerRotation(ToRotationMatrix(rotate), order);
}


void Math::TransformMatrix::SetRotation(const EulerRotation& euler)
{
    rotate = ToQuaternionRotation(euler);
}


Math::Matrix4 Math::TransformMatrix::GetMatrix() const
{
    auto rotationMatrix = ToRotationMatrix(rotate);

    Matrix4 rotateMatrix = {
            rotationMatrix.a, rotationMatrix.b, rotationMatrix.c,  0,
            rotationMatrix.d, rotationMatrix.e, rotationMatrix.f,  0,
            rotationMatrix.g, rotationMatrix.h, rotationMatrix.i,  0,
            0, 0, 0, 1

    };

    Matrix4 scaleMatrix = {
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0, 1
    };

    Matrix4 translateMatrix = {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            translate.x, translate.y, translate.z, 1
    };

    return scaleMatrix * rotateMatrix * translateMatrix;

//    Vector3 xVector = {rotationMatrix.a, rotationMatrix.b, rotationMatrix.c};
//    Vector3 yVector = {rotationMatrix.d, rotationMatrix.e, rotationMatrix.f};
//    Vector3 zVector = {rotationMatrix.g, rotationMatrix.h, rotationMatrix.i};
//
//    xVector.Normalize();
//    yVector.Normalize();
//    zVector.Normalize();

//    xVector.Multiply(scale.x);
//    yVector.Multiply(scale.y);
//    zVector.Multiply(scale.z);

//    return {
//        rotationMatrix.a * scale.x, rotationMatrix.a * scale.x, rotationMatrix.c * scale.x, 0,
//        rotationMatrix.d * scale.y, rotationMatrix.e * scale.y, rotationMatrix.f * scale.y, 0,
//        rotationMatrix.g * scale.z, rotationMatrix.h * scale.z, rotationMatrix.i * scale.z, 0,
//        translate.x, translate.y, translate.z, 1.0
//    };


}