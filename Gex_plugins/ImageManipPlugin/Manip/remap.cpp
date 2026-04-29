#include "remap.h"


QImage ImageManip::Manip::GradientRemapImage(
        const QImage& source,
        const ImageManip::Types::LinearGradient& gradient)
{

    QImage remapped(source);
    for (unsigned int y = 0; y < source.height(); y++)
    {
        const auto* sourceScanLine = reinterpret_cast<const QRgba64*>(source.scanLine(y));
        auto* scanLine = reinterpret_cast<QRgba64*>(remapped.scanLine(y));
        for (unsigned int x = 0; x < source.width(); x++)
        {
            QColor color(sourceScanLine[x]);

            double m = (color.redF() + color.greenF() + color.blueF()) / 3.0;

            QColor newColor = gradient.ColorAtStep(m);
            scanLine[x] = newColor.rgba64();
        }
    }

    return remapped;
}