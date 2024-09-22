#ifndef GEX_BLUR_H
#define GEX_BLUR_H

#include <QPixmap>

namespace ImageManip::Manip
{
    QImage BoxBlurPixmap(const QImage& pix, int steps=1);
}

#endif //GEX_BLUR_H
