#ifndef GEX_MANIP_SHAPES_H
#define GEX_MANIP_SHAPES_H

#include <QImage>


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
}

#endif //GEX_MANIP_SHAPES_H
