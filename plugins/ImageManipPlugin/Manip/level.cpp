#include "level.h"


#define CLAMP_VALUE(var, min, max) if (var < min) {var = min;} else if (var > max){var = max;}


float ApplyLevel(float src, const ImageManip::Types::LevelMap& m)
{
    if (src < m.Low())
        return m.ClampLow();
    if (src > m.High())
        return m.ClampHigh();

    if (src < m.ClampLow())
        return m.ClampLow();
    if (src > m.ClampHigh())
        return m.ClampHigh();

    return src;
}


QImage ImageManip::Manip::AdjustLevel(
        const QImage& source,
        const ImageManip::Types::LevelMap& lm)
{
    QImage dest(source);

    double destRange = lm.High() - lm.Low();
    double cmin = lm.ClampLow();
    double cmax = lm.ClampHigh();

    for (int x = 0; x < dest.width(); x++)
    {
        for (int y = 0; y < dest.height(); y++)
        {
            QColor color = dest.pixelColor(x, y);

//            double red = lm.Low() + (color.redF() * destRange);
//            CLAMP_VALUE(red, cmin, cmax)
            color.setRedF(ApplyLevel(color.redF(), lm));

//            double green = lm.Low() + (color.greenF() * destRange);
//            CLAMP_VALUE(green, cmin, cmax)
            color.setGreenF(ApplyLevel(color.greenF(), lm));

//            double blue = lm.Low() + (color.blueF() * destRange);
//            CLAMP_VALUE(blue, cmin, cmax)
            color.setBlueF(ApplyLevel(color.blueF(), lm));

            dest.setPixelColor(x, y, color);
        }
    }

    return dest;
}