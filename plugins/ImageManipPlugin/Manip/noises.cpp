#include "noises.h"

#include <random>

#include <QPainter>

typedef std::mt19937 RGen;
typedef std::uniform_int_distribution<> NormalDist;


QImage ImageManip::Manip::RandomNoise(
        int width, int height, int seed,
        bool blackAndWhite, bool alpha)
{
    QImage img(width, height, QImage::Format_RGBA64);

    RGen gen;
    gen.seed(seed);

    NormalDist dist(0, 255);

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            QColor pixelColor;
            if (blackAndWhite)
            {
                int c = dist(gen);
                pixelColor.setRgb(
                    c, c, c
                );
            }

            else
            {
                pixelColor.setRgb(
                    dist(gen),
                    dist(gen),
                    dist(gen)
                );
            }

            if (alpha)
            {
                pixelColor.setAlpha(dist(gen));
            }

            img.setPixelColor(x, y, pixelColor);
        }
    }

    return img;
}


QPixmap VoronoiCells(int width, int height, int seed)
{
    return {};
}


QPixmap VoronoiNoise(int width, int height, int seed)
{
    return {};
}


QImage ImageManip::Manip::DirtNoise(
        int width, int height, int size,
        int randomSize, int density,
        int randomDensity, int seed)
{
    RGen gen;
    gen.seed(seed);

    NormalDist xDist(0, width);
    NormalDist yDist(0, height);

    NormalDist colorDist(50, 255);
    NormalDist sizeDist(size - randomSize,
                        size + randomSize);

    NormalDist randDensity(density - randomDensity,
                           density + randomDensity);

    int rd = randDensity(gen);
    int n = (width * (rd / 100.));

    QImage image(width, height, QImage::Format_RGBA64);
    image.fill(Qt::GlobalColor::black);

    QPainter painter(&image);
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < n; i++)
    {
        int color = colorDist(gen);

        int x = xDist(gen);
        int y = yDist(gen);
        int s = sizeDist(gen);

        painter.setBrush(QColor(color, color, color));
        painter.drawEllipse(x, y, s, s);
    }

    painter.end();

    return image;
}
