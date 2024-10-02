#ifndef GEX_NOISES_H
#define GEX_NOISES_H

#include <QPixmap>


namespace ImageManip::Manip
{
    QImage RandomNoise(int width, int height, int seed,
                       bool blackAndWhite=true,
                       bool alpha=false);

    QImage VoronoiCells(int imageWidth,
                        int imageHeight,
                        int width,
                        int height,
                        int seed);

    QImage VoronoiNoise(int width, int height, int seed);

    QImage DirtNoise(int width, int height, int size,
                     int randomSize, int density,
                     int randomDensity, int seed);
}



#endif //GEX_NOISES_H
