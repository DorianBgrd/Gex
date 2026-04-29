#include "blend.h"

#include <QPixmap>
#include <QImage>



QImage ImageManip::Manip::Blend(
        const QImage& src, const QImage& bld,
        float blend, const QImage& msk)
{

    int srcWidth = src.width();
    int dstWidth = bld.width();

    int srcHeight = src.height();
    int dstHeight = bld.height();

    int width = srcWidth;
    if (dstWidth < srcWidth)
        width = dstWidth;

    int height = srcHeight;
    if (dstHeight < srcHeight)
        height = dstHeight;

    float factor = 1 + blend;

    QImage blended(src);
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            QColor pixelBlendColor = msk.pixelColor(x, y);
            float pixelBlend = (
                    (pixelBlendColor.redF() + pixelBlendColor.greenF() +
                    pixelBlendColor.blueF()) / 3.f);

            QColor srcColor = src.pixelColor(x, y);
            QColor dstColor = bld.pixelColor(x, y);

            float pblend = blend * pixelBlend;

//            QColor dest(
//                    (srcColor.red() + (dstColor.red() * pblend)) / 1 + pblend,
//                    (srcColor.green() + (dstColor.green() * pblend)) / 1 + pblend,
//                    (srcColor.blue() + (dstColor.blue() * pblend)) / 1 + pblend,
//                    (srcColor.alpha() + (dstColor.alpha() * pblend)) / 1 + pblend
//            );
            QColor dest (
                    srcColor.red() * (1 - pblend) + dstColor.red() * pblend,
                    srcColor.green() * (1 - pblend) + dstColor.green() * pblend,
                    srcColor.blue() * (1 - pblend) + dstColor.blue() * pblend,
                    srcColor.alpha() * (1 - pblend) + dstColor.alpha() * pblend
            );

            blended.setPixelColor(x, y, dest);
        }
    }

    return blended;
}


QPainter::CompositionMode ImageManip::Manip::GetCompositionMode(std::string mode)
{
    if (mode == "Normal")
        return QPainter::CompositionMode_DestinationOver;

    auto iter = std::find(
            CompositionModeNames.begin(),
            CompositionModeNames.end(),
            mode);

    unsigned int index = std::distance(
            CompositionModeNames.begin(),
            iter) - 1;

    return QPainter::CompositionMode(index + cmpoffset);
}

QImage ImageManip::Manip::LayerBlend(
        const QImage& source, const QImage& blend,
        std::string composition)
{
    auto mode = GetCompositionMode(composition);

    QImage dest(source);

    QPainter painter(&dest);

    painter.setCompositionMode(mode);
    painter.drawImage(0, 0, blend);

    painter.end();

    return dest;
}


QImage ImageManip::Manip::ApplyRGBMask(
        const QImage& source, const QImage& mask)
{
    QImage dest(source);

    for (unsigned int x = 0; x < source.width(); x++)
    {
        for (unsigned int y = 0; y < source.height(); y++)
        {
            QColor maskColor = mask.pixelColor(x, y);
            QColor inputColor = source.pixelColor(x, y);
            auto alpha = (maskColor.red() + maskColor.green() + maskColor.blue()) / 255;

            inputColor.setAlpha(alpha);
            dest.setPixelColor(x, y, inputColor);
        }
    }

    return dest;
}


QImage ImageManip::Manip::ApplyAlphaMask(
        const QImage& source, const QImage& mask)
{
    QImage dest(source);

    for (unsigned int x = 0; x < source.width(); x++)
    {
        for (unsigned int y = 0; y < source.height(); y++)
        {
            QColor maskColor = mask.pixelColor(x, y);
            QColor inputColor = source.pixelColor(x, y);

            inputColor.setAlpha(maskColor.alpha());
            dest.setPixelColor(x, y, inputColor);
        }
    }

    return dest;
}