#ifndef GEX_MANIP_CHANNELS_H
#define GEX_MANIP_CHANNELS_H

#include <QImage>

namespace ImageManip::Manip
{
    QImage ComposeImage(const QImage& red, const QImage& green,
                        const QImage& blue, const QImage& alpha);

    void DecomposeImage(const QImage& source, QImage& red,
                        QImage& green, QImage& blue,
                        QImage& alpha);
}

#endif //GEX_MANIP_CHANNELS_H
