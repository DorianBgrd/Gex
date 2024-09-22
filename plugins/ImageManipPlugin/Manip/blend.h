#ifndef GEX_BLEND_H
#define GEX_BLEND_H

#include <QPixmap>
#include <QPainter>


namespace ImageManip::Manip
{
    QImage Blend(const QImage& pix1, const QImage& pix2,
                 float blend, const QImage& mask);

    static std::array<std::string, 13> CompositionModeNames = {
            "Normal",
            "Plus",
            "Multiply",
            "Screen",
            "Overlay",
            "Darken",
            "Lighten",
            "ColorDodge",
            "ColorBurn",
            "HardLight",
            "SoftLight",
            "Difference",
            "Exclusion"
    };

    static unsigned int cmpoffset = 12;

    QPainter::CompositionMode GetCompositionMode(std::string mode);

    QImage LayerBlend(const QImage& pix1, const QImage& pix2,
                      std::string composition);

    QImage ApplyRGBMask(const QImage& source, const QImage& mask);

    QImage ApplyAlphaMask(const QImage& source, const QImage& mask);
}

#endif //GEX_BLEND_H
