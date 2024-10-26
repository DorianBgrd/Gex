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

    for (int y = 0; y < dest.height(); y++)
    {
        auto* scanLine = reinterpret_cast<QRgba64*>(dest.scanLine(y));

        for (int x = 0; x < dest.width(); x++)
        {
            QColor pixelColor(scanLine[x]);
            pixelColor.setRedF(ApplyLevel(pixelColor.redF(), lm));
            pixelColor.setGreenF(ApplyLevel(pixelColor.greenF(), lm));
            pixelColor.setBlueF(ApplyLevel(pixelColor.blueF(), lm));

            scanLine[x] = pixelColor.rgba64();
        }
    }

    return dest;
}
