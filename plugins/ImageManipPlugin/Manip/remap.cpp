#include "remap.h"


QImage ImageManip::Manip::GradientRemapImage(
        const QImage& source,
        const ImageManip::Types::LinearGradient& gradient)
{

    QImage remapped(source);
    for (unsigned int x = 0; x < source.width(); x++)
    {
        for (unsigned int y = 0; y < source.height(); y++)
        {
            QColor color = source.pixelColor(x, y);

            double m = (color.redF() + color.greenF() + color.blueF()) / 3.0;

            QColor newColor = gradient.ColorAtStep(m);
            remapped.setPixelColor(x, y, newColor);
        }
    }

    return remapped;
}