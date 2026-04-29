#include "tile.h"

#include <QPainter>


QImage ImageManip::Manip::TileImage(const QImage& image, int tileNumber)
{
    if (tileNumber <= 0)
        return image;

    int width = image.width();
    int height = image.height();

    QImage result(QSize(width, height), QImage::Format_RGBA64);
    result.fill(Qt::black);

    QPainter painter(&result);

    tileNumber += 1;

    int xDelta = width / tileNumber;
    int yDelta = height / tileNumber;

    for (int i = 0; i < tileNumber; i++)
    {
        for (int u = 0; u < tileNumber; u++)
        {
            painter.drawImage(
                    QRectF(i * xDelta, u * yDelta,
                        xDelta, yDelta), image,
                    QRectF(0, 0, width, height)
                    );
        }
    }

    painter.end();

    return result;
}

