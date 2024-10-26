#include "channels.h"



QImage ImageManip::Manip::ComposeImage(
        const QImage& red, const QImage& green,
        const QImage& blue, const QImage& alpha)
{
    int width = red.width();
    if (green.width() < width)
        width = green.width();
    if (blue.width() < width)
        width = blue.width();

    if (!alpha.isNull() && alpha.width() < width)
        width = alpha.width();

    int height = red.height();
    if (green.height() < height)
        height = green.height();
    if (blue.height() < height)
        height = blue.height();

    if (!alpha.isNull() && alpha.height() < height)
        height = alpha.height();

    QImage result(width, height, QImage::Format_RGBA64);
    for (int y = 0; y < height; y++)
    {
//        auto* scanLine = result.scanLine(y);
        auto* line = reinterpret_cast<QRgba64*>(result.scanLine(y));
        auto* redLine = reinterpret_cast<const QRgba64*>(red.scanLine(y));
        auto* greenLine = reinterpret_cast<const QRgba64*>(green.scanLine(y));
        auto* blueLine = reinterpret_cast<const QRgba64*>(blue.scanLine(y));

        const QRgba64* alphaLine = nullptr;
        if (!alpha.isNull())
            alphaLine = reinterpret_cast<const QRgba64*>(alpha.scanLine(y));

        for (int x = 0; x < width; x++)
        {
            const QColor redRgba = redLine[x];
            const QColor greenRgba = greenLine[x];
            const QColor blueRgba = blueLine[x];

            QColor alphaRgba(0, 0, 0, 255);
            if (alphaLine)
            {
                alphaRgba = alphaLine[x];
            }

            line[x] = QColor(redRgba.red(), greenRgba.green(),
                             blueRgba.blue(), alphaRgba.alpha()).rgba64();
        }
    }

    return result;
}


void ImageManip::Manip::DecomposeImage(
        const QImage& source, QImage& red,
        QImage& green, QImage& blue,
        QImage& alpha)
{
    int width = source.width();
    int height = source.height();

    for (int y = 0; y < height; y++)
    {
        const uchar *sourceLine = source.constScanLine(y);
        auto* redLine = red.scanLine(y);
        auto* greenLine = green.scanLine(y);
        auto* blueLine = blue.scanLine(y);
        auto* alphaLine = alpha.scanLine(y);

        for (int x = 0; x < width; x++)
        {
            QColor sourcePixelColor(((QRgba64 *)sourceLine)[x]);

            QColor redColor(sourcePixelColor.red(), 0, 0, 255);
            ((QRgba64 *)redLine)[x] = redColor.rgba64();

            QColor greenColor(0, sourcePixelColor.green(), 0, 255);
            ((QRgba64 *)greenLine)[x] = greenColor.rgba64();

            QColor blueColor(0, 0, sourcePixelColor.blue(), 255);
            ((QRgba64 *)blueLine)[x] = blueColor.rgba64();

            QColor alphaColor(255, 255, 255, sourcePixelColor.alpha());
            ((QRgba64 *)alphaLine)[x] = alphaColor.rgba64();
        }
    }
}
