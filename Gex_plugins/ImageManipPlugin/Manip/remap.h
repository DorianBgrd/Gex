#ifndef GEX_MANIP_REMAP_H
#define GEX_MANIP_REMAP_H

#include <QImage>
#include "Types/gradient.h"


#include "../export.h"

namespace ImageManip::Manip
{
    Plugin_API
    QImage GradientRemapImage(
            const QImage& source,
            const ImageManip::Types::LinearGradient& gradient
            );
}

#endif //GEX_MANIP_REMAP_H
