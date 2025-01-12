#ifndef GEX_MANIP_SHAPES_H
#define GEX_MANIP_SHAPES_H

#include <QImage>
#include <QPoint>
#include <vector>
#include "Types/geometry.h"


namespace ImageManip::Manip
{
    QImage GenerateRectangle(int imageWidth,
                             int imageHeight,
                             int x,
                             int y,
                             int width,
                             int height,
                             const QColor& color);


    QImage GenerateEllipse(int imageWidth,
                           int imageHeight,
                           int x,
                           int y,
                           int width,
                           int height,
                           const QColor& color);

    QImage GeneratePolygon(int imageWidth,
                           int imageHeight,
                           const std::vector<Types::Point>& points,
                           const QColor& color,
                           const QImage& source=QImage());
}

#endif //GEX_MANIP_SHAPES_H
