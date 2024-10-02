#include "noises.h"

#include <random>
#include <set>
#include <map>

#include <QPainter>
#include <QPainterPath>

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


struct GCoord
{
    int x;
    int y;
};


struct GRect
{
    int width;
    int height;
};


typedef std::array<int, 2> Coord2D;
typedef std::array<double, 2> DCoord2D;
typedef std::array<double, 3> DCoord3D;


struct GCell
{
    int x = 0;
    int y = 0;
    double px = 0;
    double py = 0;

public:
    GCell() = default;

    GCell(int x_, int y_, double px_, double py_)
    {
        x = x_;
        y = y_;
        px = px_;
        py = py_;
    }

    GCell(const GCell& other)
    {
        x = other.x;
        y = other.y;
        px = other.px;
        py = other.py;
    }


    GCell(int x_, int y_, const GCell& other)
    {
        x = x_;
        y = y_;
        px = other.px;
        py = other.py;
    }

    DCoord2D RandPoint() const
    {
        return {px, py};
    }

    DCoord2D RandRealPoint() const
    {
        return {x + px, y + py};
    }
};



float Vec2DLength(DCoord2D p1, DCoord2D p2)
{
    return std::sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) +
                     (p1[1] - p2[1]) * (p1[1] - p2[1]));
}


float TArea2D(DCoord2D p1, DCoord2D p2, DCoord2D p3)
{
    float a = Vec2DLength(p1, p2);
    float b = Vec2DLength(p2, p3);
    float c = Vec2DLength(p3, p1);
    float d = (a + b + c) / 2.0f;

    return std::sqrt(d * (d - a) * (d - b) * (d - c));
}


DCoord3D BCoord2D(DCoord2D point, DCoord2D p1, DCoord2D p2, DCoord2D p3)
{
    float mainArea = TArea2D(p1, p2, p3);

    float area1 = TArea2D(point, p2, p3);
    float area2 = TArea2D(p1, point, p3);
    float area3 = TArea2D(p1, p2, point);

    float darea1 = area1 / mainArea;
    float darea2 = area2 / mainArea;
    float darea3 = area3 / mainArea;

    return {darea1, darea2, darea3};
}


DCoord2D FromBCoord2D(DCoord3D bary, DCoord2D p1, DCoord2D p2, DCoord2D p3)
{
    return {
            (bary[0] * p1[0]) + (bary[1] * p2[0]) + (bary[2] * p3[0]),
            (bary[0] * p1[1]) + (bary[1] * p2[1]) + (bary[2] * p3[1])
    };
}


struct Triangle2D
{
    DCoord2D p1;
    DCoord2D p2;
    DCoord2D p3;

    DCoord2D rp;

    Triangle2D(
            DCoord2D p1_,
            DCoord2D p2_,
            DCoord2D p3_
            )
    {
        p1 = p1_;
        p2 = p2_;
        p3 = p3_;
    }

    Triangle2D(const Triangle2D& other)
    {
        p1 = other.p1;
        p2 = other.p2;
        p3 = other.p3;

        rp = other.rp;
    }

    void GenerateRandomPoint(double b1, double b2, double b3)
    {
        rp = FromBCoord2D({b1, b2, b3}, p1, p2, p3);
    }

    static Triangle2D FromCoord(DCoord2D p1, DCoord2D p2,
                                DCoord2D p3, double x,
                                double y, double z)
    {
        Triangle2D triangle(p1, p2, p3);
        triangle.GenerateRandomPoint(x, y, z);

        return triangle;
    }

    bool operator ==(const Triangle2D& other) const
    {
        return (p1[0] == other.p1[0] && p1[1] == other.p1[1] &&
                p2[0] == other.p2[0] && p2[1] == other.p2[1] &&
                p3[0] == other.p3[0] && p3[1] == other.p3[1]);
    }

    bool operator <(const Triangle2D& other) const
    {
        return (p1 < other.p1 && p2 < other.p2 && p3 < other.p3);
    }
};


DCoord2D RealCoord(int x, int y, Coord2D point)
{
    return {static_cast<double>(x + point[0]),
            static_cast<double>(y + point[1])};
}


DCoord2D RealCoord(int x, int y, DCoord2D point)
{
    return {static_cast<double>(x + point[0]),
            static_cast<double>(y + point[1])};
}


struct VoronoiCell
{
    std::vector<DCoord2D> coords;
};


QImage ImageManip::Manip::VoronoiCells(int imageWidth,
                     int imageHeight,
                     int gridWidth,
                     int gridHeight,
                     int seed)
{
    QImage res(imageWidth, imageHeight, QImage::Format_RGBA64);
    res.fill(Qt::black);

    QPainter painter(&res);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen;

    RGen gen;
    gen.seed(seed);

    std::uniform_real_distribution<double> dist(0, 1);

    std::map<Coord2D, GCell> cellsPoints;
    for (int x = 0; x < gridWidth; x++)
    {
        for (int y = 0; y < gridHeight; y++)
        {
            auto rx = dist(gen);
            auto ry = dist(gen);
            Coord2D c = {x, y};
            cellsPoints[c] = GCell(
                    x, y,
                    rx, ry
            );
        }
    }

    // Expand grid by one to make it tillable.
    for (int i = 0; i < gridWidth; i++)
    {
        cellsPoints[{i, -1}] = GCell(
                i, -1, cellsPoints.at({i, gridHeight - 1})
                );

        cellsPoints[{i, gridHeight}] = GCell(
                i, gridHeight, cellsPoints.at({i, 0})
                );
    }

    for (int j = 0; j < gridHeight; j++)
    {
        cellsPoints[{-1, j}] = GCell(
                -1, j, cellsPoints.at({gridWidth - 1, j})
                );

        cellsPoints[{gridWidth, j}] = GCell(
                gridWidth, j, cellsPoints.at({0, j})
                );
    }

    cellsPoints[{-1, -1}] = GCell(-1, -1, cellsPoints.at({gridWidth - 1, gridHeight - 1}));
    cellsPoints[{gridWidth, gridHeight}] = GCell(gridWidth, gridHeight, cellsPoints.at({0, 0}));
    cellsPoints[{-1, gridHeight}] = GCell(-1, gridHeight, cellsPoints.at({gridWidth - 1, 0}));
    cellsPoints[{gridWidth, -1}] = GCell(gridWidth, -1, cellsPoints.at({-1, gridHeight}));

    pen.setColor(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);

    for (auto cp : cellsPoints)
    {
        auto cprp = cp.second.RandRealPoint();
        auto cprpx = static_cast<int>(cprp[0] / gridWidth * imageWidth);
        auto cprpy = static_cast<int>(cprp[1] / gridHeight * imageHeight);
        painter.drawPoint(cprpx, cprpy);

    }

    std::vector<Triangle2D> triangles;

    std::uniform_real_distribution<double> rdist(0, 1);

    std::vector<VoronoiCell> cells;
    for (int x = 0; x < gridWidth + 1; x++)
    {
        for (int y = 0; y < gridHeight + 1; y++)
        {
            Coord2D center = {x, y};

            Coord2D topLeft = {x - 1, y - 1};
            Coord2D topCenter = {x, y - 1};
            Coord2D topRight = {x + 1, y - 1};
            Coord2D left = {x - 1, y};
            Coord2D right = {x + 1, y};
            Coord2D bottomLeft = {x - 1, y + 1};
            Coord2D bottomCenter = {x, y + 1};
            Coord2D bottomRight = {x + 1, y + 1};

            // Triangles :
            std::vector<std::array<Coord2D, 3>> ts = {
                    {topLeft, topCenter, center},
//                    {topCenter, topRight, center},
//                    {topRight, right, center},
//                    {right, bottomRight, center},
//                    {bottomRight, bottomCenter, center},
//                    {bottomCenter, bottomLeft, center},
//                    {bottomLeft, left, center},
                    {left, topLeft, center}
            };

            // Get real coordinates.
            VoronoiCell vcell;
            for (auto tsg : ts)
            {
                // Generate a random double between 0 and 1.
                auto rx = rdist(gen);

                // Divide the rest using another random
                // generated number.
                auto factor = rdist(gen);
                auto ry = (1 - rx) * factor;
                auto rz = (1 - rx) * (1 - factor);

                // Create a triangle.
                auto triangle = Triangle2D::FromCoord(
                        cellsPoints.at(tsg[0]).RandRealPoint(),
                        cellsPoints.at(tsg[1]).RandRealPoint(),
                        cellsPoints.at(tsg[2]).RandRealPoint(),
                        rx, ry, rz);

                auto foundIter = std::find(triangles.begin(), triangles.end(), triangle);
                if (foundIter == triangles.end())
                    triangles.push_back(triangle);
//                else
//                    triangle = *foundIter;

//                vcell.coords.push_back(triangle.rp);
            }

//            cells.push_back(vcell);
        }
    }



    pen.setColor(Qt::green);
    pen.setWidth(2);
    painter.setPen(pen);

    std::uniform_int<int> cdist(0, 255);
    for (auto t : triangles)
    {
        QPainterPath path;
        path.moveTo(t.p1[0] / gridWidth * imageWidth,
                    t.p1[1] / gridHeight * imageHeight);
        path.lineTo(t.p2[0] / gridWidth * imageWidth,
                    t.p2[1] / gridHeight * imageHeight);
        path.moveTo(t.p2[0] / gridWidth * imageWidth,
                    t.p2[1] / gridHeight * imageHeight);
        path.lineTo(t.p3[0] / gridWidth * imageWidth,
                    t.p3[1] / gridHeight * imageHeight);
        path.moveTo(t.p3[0] / gridWidth * imageWidth,
                    t.p3[1] / gridHeight * imageHeight);
        path.lineTo(t.p1[0] / gridWidth * imageWidth,
                    t.p1[1] / gridHeight * imageHeight);

        QColor arbcolor = QColor(cdist(gen), cdist(gen), cdist(gen));

        pen.setColor(arbcolor);
        painter.setPen(pen);
        painter.setBrush(QBrush(arbcolor));

        painter.drawPath(path);
    }

    pen.setColor(Qt::white);
    pen.setWidth(2);

    painter.setPen(pen);
    for (auto cell : cells)
    {
        QPainterPath path;
        path.moveTo(cell.coords.at(0)[0],
                    cell.coords.at(0)[1]);
        for (unsigned int i = 1; i < cell.coords.size(); i++)
        {
            auto cc = cell.coords.at(i);

            auto x = (cc[0] / gridWidth) * imageWidth;
            auto y = (cc[1] / gridHeight) * imageHeight;
            path.lineTo(x, y);
            path.moveTo(x, y);
        }

//        painter.drawPath(path);
    }

    painter.end();
    return res;
}


QImage ImageManip::Manip::VoronoiNoise(int width, int height, int seed)
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
