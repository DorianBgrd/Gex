#ifndef GEX_XFORM_H
#define GEX_XFORM_H


#include "Gex_plugins/MathPlugin/matrix.h"


namespace Geometry
{
    class Xform
    {
        Matrix::Matrix4 transform;

    public:
        Xform();

        virtual ~Xform() = default;
    };
}

#endif //GEX_XFORM_H
