#ifndef GEX_TRANSFORMMATRIX_H
#define GEX_TRANSFORMMATRIX_H

#include "Gex_plugins/MathPlugin/matrix.h"
#include "Gex_plugins/MathPlugin/vector.h"
#include "Gex_plugins/MathPlugin/rotation.h"

namespace Math
{
    class TransformMatrix
    {
        Vector3 translate;
        Quaternion rotate;
        Vector3 scale;

    public:
        TransformMatrix(const Matrix4& mat);

        Vector3 GetTranslation() const;

        void SetTranslation(const Vector3& translate);

        Vector3 GetScale() const;

        void SetScale(const Vector3& scale);

        Quaternion GetRotation() const;

        void SetRotation(const Quaternion& quat);

        EulerRotation GetEulerRotation(const RotateOrder& order) const;

        void SetRotation(const EulerRotation& euler);

        Matrix4 GetMatrix() const;
    };
}

#endif //GEX_TRANSFORMMATRIX_H
