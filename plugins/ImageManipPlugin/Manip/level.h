#ifndef GEX_MANIP_LEVEL_H
#define GEX_MANIP_LEVEL_H

#include <QImage>
#include "Types/level.h"

namespace ImageManip::Manip
{
    QImage AdjustLevel(const QImage& source, const ImageManip::Types::LevelMap& lm);
}

#endif //GEX_MANIP_LEVEL_H
