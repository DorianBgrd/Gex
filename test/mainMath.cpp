
#include "Gex_plugins/MathPlugin/matrix.h"
#include "Gex_plugins/MathPlugin/rotation.h"
#include "Gex_plugins/GeometryPlugin/transformMatrix.h"


int main(int argc, char** argv)
{
    auto identity = Math::GetIdentity();
    auto identityDet = Math::GetDeterminant(identity);

    Math::Matrix4 test1 = {
            0, 0, 1, 0,
            0, 1, 0, 0,
            -1, 0, 0, 0,
            1, 2, 3, 1
    };

    auto inverted = Math::GetInverse(identity);

    auto mult = identity * inverted;

    auto test1Inverted = Math::GetInverse(test1);

    auto test1Mult = test1 * test1Inverted;


    Math::EulerRotation euler;
    euler.SetDegrees(45, 68, 95);

    Math::Quaternion quat = Math::ToQuaternionRotation(euler);

    Math::Matrix3 rotMat = Math::ToRotationMatrix(quat);

    Math::EulerRotation euler2 = Math::ToEulerRotation(quat, euler.order);

    Math::Quaternion quat2 = Math::ToQuaternionRotation(rotMat);


    Math::TransformMatrix tmat(Math::GetIdentity());

    tmat.SetTranslation({0, 10, 20});

    Math::EulerRotation euler3 = {Math::DegToRad(45), Math::DegToRad(90), Math::DegToRad(180), Math::RotateOrder::XYZ};
    tmat.SetRotation(euler3);

    Math::EulerRotation justSet = tmat.GetEulerRotation(Math::RotateOrder::XYZ);

    tmat.SetScale({0.5, 0.75, 1.0});

    auto tmatResult = tmat.GetMatrix();

    Math::TransformMatrix tmat2(tmatResult);

    auto translate = tmat2.GetTranslation();

    auto rotate = tmat2.GetEulerRotation(Math::RotateOrder::XYZ);

    auto scale = tmat2.GetScale();

    return 0;
}
