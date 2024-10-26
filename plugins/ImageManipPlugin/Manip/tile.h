#ifndef GEX_MANIP_TILE_H
#define GEX_MANIP_TILE_H

#include <QImage>

#include "plugins/export.h"


namespace ImageManip::Manip
{
    QImage Plugin_API TileImage(const QImage& image, int tileNumber);
}

#endif //GEX_MANIP_TILE_H
