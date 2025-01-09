#ifndef GEX_NOISES_H
#define GEX_NOISES_H

#include <QPixmap>
#include <vector>

#include "Types/geometry.h"


namespace ImageManip::Manip
{

    QImage RandomNoise(int width, int height, int seed,
                       bool blackAndWhite=true,
                       bool alpha=false);

    class Cell
    {
    public:
        double px = 0;
        double py = 0;

    public:
        Cell() = default;

        Cell(const Cell& other);

        Cell(double px, double py);
    };


    class Grid
    {
        int width = 0;
        int height = 0;

        std::vector<std::vector<Cell>> grid;

    public:
        Grid() = default;

        Grid(int resolution, int seed);

        Grid(int width, int height, int seed);

        std::vector<Cell> Cells() const;

        void Tile();

        void TileRepeat();

        std::vector<Types::Point> MapPoints(
                double x, double y, double cellWidth,
                double cellHeight) const;

        int ColumnCount() const;

        int RowCount() const;
    };


    std::vector<Types::Triangle> DelaunayTriangulation(
            const std::vector<Types::Point>& points,
            const Types::Triangle& superTriangle
            );

    QImage DelaunayNoise(int imageWidth,
                         int imageHeight,
                         int frequency,
                         int seed,
                         bool colored=false,
                         bool tileable=true
                         );


    QImage VoronoiNoise(int imageWidth,
                        int imageHeight,
                        int frequency,
                        int seed,
                        int lineWidth=1,
                        bool filled=true,
                        bool colored=false,
                        bool tileable=true);

    QImage DirtNoise(int width, int height, int size,
                     int randomSize, int density,
                     int randomDensity, int seed);

    QImage TriangleCirconscrit(int imageWidth, int imageHeight, int seed);

    QImage GridViz(int imageWidth,
                   int imageHeight,
                   int width,
                   int height,
                   int seed,
                   bool tileable);

    QImage CircallyOrdered(int imageWidth,
                           int imageHeight,
                           int frequency,
                           int seed);
}





#endif //GEX_NOISES_H
