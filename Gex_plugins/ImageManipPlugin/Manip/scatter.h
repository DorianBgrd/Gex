#include <QImage>


namespace ImageManip::Manip
{
    QImage Scatter(
            const QImage& source,
            const QImage& scatter,
            int frequency=10,
            int seed=0,
            bool randomRotation=true,
            float randomScale=false,
            float minScale=0.1,
            float maxScale=2.0
    );
}