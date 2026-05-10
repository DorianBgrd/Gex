#include "scatter.h"

#include <random>
#include <set>
#include <map>

#include <QPainter>


typedef std::mt19937 RGen;
typedef std::uniform_int_distribution<> NormalDist;
typedef std::uniform_real_distribution<float> FloatDist;


QImage ImageManip::Manip::Scatter(
        const QImage& source,
        const QImage& scatter,
        int frequency,
        int seed,
        bool randomRotation,
        float randomScale,
        float minScale,
        float maxScale
)
{
    RGen gen;
    gen.seed(seed);

    NormalDist xDist(0, source.width() - 1);
    NormalDist yDist(0, source.width() - 1);
    FloatDist scaleDist(minScale, maxScale);

    FloatDist rotationDict(0, 360);

    QPixmap pixmap = QPixmap::fromImage(source);

    QPainter painter(&pixmap);

    if (randomRotation)
        painter.rotate(rotationDict(gen));

    for (int f = 0; f < frequency; f++)
    {
        QPixmap drawPixmap = pixmap;
        if (randomScale)
        {
            float scale = scaleDist(gen);
            drawPixmap = pixmap.scaled(
                scatter.width() * scale,
                scatter.height() * scale
            );

            painter.drawPixmap(
                    xDist(gen),
                    yDist(gen),
                    drawPixmap
            );
        }
    }

    painter.end();

    return pixmap.toImage();
}