#include "noises.h"


#include <iostream>

#include <random>
#include <set>
#include <map>

#include <QPainter>
#include <QPainterPath>

typedef std::mt19937 RGen;
typedef std::uniform_int_distribution<> NormalDist;
typedef std::uniform_real_distribution<double> DoubleDist;


QImage ImageManip::Manip::RandomNoise(
        int width, int height, int seed,
        bool blackAndWhite, bool alpha)
{
    QImage img(width, height, QImage::Format_RGBA64);

    RGen gen;
    gen.seed(seed);

    NormalDist dist(0, 255);

    for (int y = 0; y < height; y++)
    {
        auto* scanLine = reinterpret_cast<QRgba64*>(img.scanLine(y));

        for (int x = 0; x < width; x++)
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

            scanLine[x] = pixelColor.rgba64();
        }
    }

    return img;
}



ImageManip::Manip::Cell::Cell(const Cell& other)
{
    px = other.px;
    py = other.py;
}


ImageManip::Manip::Cell::Cell(double x, double y)
{
    px = x;
    py = y;
}


//ImageManip::Manip::Point ImageManip::Manip::Cell::GetPoint() const
//{
//    return {x + px * width,
//            y + py * height};
//}
//
//
//ImageManip::Manip::Point ImageManip::Manip::Cell::GeneratePoint(double xFactor, double yFactor)
//{
//    px = xFactor;
//    py = yFactor;
//    return GetPoint();
//}
//
//
//void ImageManip::Manip::Cell::Update(double x_, double y_)
//{
//    x = x_;
//    y = y_;
//}


ImageManip::Manip::Grid::Grid(int w, int h, int seed)
{
    width = w;
    height = h;

    RGen gen;
    gen.seed(seed);

    std::uniform_real_distribution<double> pointDist(0, 1);

    for (unsigned int x = 0; x < width; x++)
    {
        std::vector<Cell> column;
        for (unsigned int y = 0; y < height; y++)
        {
            column.emplace_back(pointDist(gen), pointDist(gen));
        }

        grid.push_back(column);
    }
}


ImageManip::Manip::Grid::Grid(int r, int seed): Grid(r, r, seed)
{

}


std::vector<ImageManip::Manip::Cell> ImageManip::Manip::Grid::Cells() const
{
    std::vector<Cell> cells;
    for (const std::vector<Cell>& column : grid)
    {
        for (const Cell& cell : column)
        {
            cells.push_back(cell);
        }
    }

    return cells;
}


void ImageManip::Manip::Grid::Tile()
{
    // Prepare grid corners.
    auto topLeft = grid.at(width - 1).at(height - 1);
    auto topRight = grid.at(0).at(height - 1);
    auto bottomLeft = grid.at(width - 1).at(0);
    auto bottomRight = grid.at(0).at(0);

    for (auto& column : grid)
    {
        auto top = column.at(0);
        auto bottom = column.at(column.size() - 1);

        column.insert(column.begin(), bottom);
        column.push_back(top);
    }

    std::vector<Cell> columnRight = grid.at(0);
    std::vector<Cell> columnLeft = grid.at(grid.size() - 1);

    grid.insert(grid.begin(), columnLeft);
    grid.push_back(columnRight);

    width += 2;
    height += 2;

    grid[0][0] = topLeft;
    grid[width - 1][0] = topRight;
    grid[0][height - 1] = bottomLeft;
    grid[width - 1][height - 1] = bottomRight;
}


void ImageManip::Manip::Grid::TileRepeat()
{
    /*
     * [
     *      [x, x, x, x],
     *      [x, x, x, x],
     *      [x, x, x, x],
     *      [x, x, x, x]
     * ]
     */

    /*
     * [
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x]
     *
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x]
     *
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x],
     *      [x, x, x, x]   [x, x, x, x]   [x, x, x, x]
     * ]
     */

    /*
     * [
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     * ]
     */

    /*
     * [
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     *      [x, x, x, x, x, x, x, x, x, x, x, x],
     * ]
     */

    std::vector<std::vector<Cell>> originGrid(grid);

    // Start by tripling the rows.
    for (auto& row : originGrid)
    {
        std::vector<Cell> copy = row;

        // Copy it twice.
        row.insert(row.end(), copy.begin(), copy.end());
    }

    std::vector<std::vector<Cell>> originCopy(originGrid);
    originGrid.insert(originGrid.end(), originCopy.begin(), originCopy.end());

    grid = originGrid;
}


std::vector<ImageManip::Types::Point> ImageManip::Manip::Grid::MapPoints(
        double x, double y, double cellWidth,
        double cellHeight) const
{
    std::vector<Types::Point> points;

    int cx = 0;
    for (const std::vector<Cell>& column : grid)
    {
        int cy = 0;
        for (const Cell& cell : column)
        {
            double cellX = x + cx * cellWidth;
            double cellY = y + cy * cellHeight;
            points.emplace_back(
                    cellX + cell.px * cellWidth,
                    cellY + cell.py * cellHeight
                    );
            cy++;
        }
        cx++;
    }

    return points;
}


int ImageManip::Manip::Grid::ColumnCount() const
{
    return width;
}


int ImageManip::Manip::Grid::RowCount() const
{
    return height;
}


QImage ImageManip::Manip::GridViz(
        int imageWidth, int imageHeight,
        int width, int height,
        int seed, bool tileable)
{
    QImage resultImage(imageWidth, imageHeight,
                       QImage::Format_RGBA64);
    resultImage.fill(Qt::black);

    Grid grid(width, height, seed);
    if (tileable)
    {
        grid.Tile();
    }

    double xStart = 0;
    double yStart = 0;

    QPainter painter(&resultImage);

    QPen cellRectPen("green");
    cellRectPen.setWidth(3);

    QBrush cellPointBrush("red");

    double cellWidth = imageWidth / grid.ColumnCount();
    double cellHeight = imageHeight / grid.RowCount();

    auto points = grid.MapPoints(0, 0, cellWidth, cellHeight);
    for (int x = 0; x < grid.ColumnCount(); x++)
    {
        for (int y=0; y < grid.RowCount(); y++)
        {
            painter.setPen(cellRectPen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(x * cellWidth, y * cellHeight,
                             cellWidth, cellHeight);

            painter.setPen(Qt::NoPen);
            painter.setBrush(cellPointBrush);

            auto p = points.at(x * grid.ColumnCount() + y);
            painter.setPen(Qt::NoPen);
            painter.setBrush(cellPointBrush);
            painter.drawEllipse(QPoint(p.x, p.y), 5, 5);
        }
    }

//    for (const Cell& cell : grid.Cells())
//    {
//        painter.setPen(cellRectPen);
//        painter.setBrush(Qt::NoBrush);
//        painter.drawRect(cell.x, cell.y, cell.width, cell.height);
//
//        auto cellPoint = cell.GetPoint();
//
//        painter.setPen(Qt::NoPen);
//        painter.setBrush(cellPointBrush);
//        painter.drawEllipse(QPoint(cellPoint.x, cellPoint.y), 5, 5);
//    }

    painter.end();

    return resultImage;
}


bool _Matches(ImageManip::Types::Triangle t1,
              ImageManip::Types::Triangle t2)
{
    for (const ImageManip::Types::Segment& seg : t1.Segments())
    {
        if (t2.HasSegment(seg))
            return true;
    }

    return false;
}


bool _PointMatch(ImageManip::Types::Triangle t1,
                 ImageManip::Types::Triangle t2)
{
    auto t1Points = t1.Points();
    auto t2Points = t2.Points();
    for (auto p : t1Points)
    {
        if (std::find(t2Points.begin(), t2Points.end(), p) != t2Points.end())
        {
            return true;
        }
    }

    return false;
}




std::vector<ImageManip::Types::Triangle> AddPoint(
        ImageManip::Types::Point point,
        std::vector<ImageManip::Types::Triangle> triangles)
{
    std::vector<ImageManip::Types::Triangle> res;
    std::vector<ImageManip::Types::Segment> edges;

    for (const ImageManip::Types::Triangle& triangle : triangles)
    {
        auto circum = CircumCircle(triangle.a, triangle.b, triangle.c);
        if  (IsInsideCircle(circum, point))
        {
            for (const ImageManip::Types::Segment& edge : triangle.Segments())
            {
                edges.push_back(edge);
            }
        }
        else
        {
            res.push_back(triangle);
        }
    }

    std::vector<ImageManip::Types::Segment> polygon;
    for (const auto& edge : edges)
    {
        if (std::count(edges.begin(), edges.end(), edge) > 1)
            continue;
        res.emplace_back(edge.p1, edge.p2, point);
    }

    return res;
}


std::vector<ImageManip::Types::Triangle> ImageManip::Manip::DelaunayTriangulation(
        const std::vector<Types::Point>& points,
        const Types::Triangle& superTriangle
)
{
    std::vector<Types::Triangle> triangles = {superTriangle};

    for (const auto& point : points)
    {
        triangles = AddPoint(point, triangles);
    }

    std::vector<Types::Triangle> res;
    for (const auto& t : triangles)
    {
        if (_Matches(t, superTriangle))
        {
            continue;
        }

        if (_PointMatch(t, superTriangle))
        {
            continue;
        }

        res.push_back(t);
    }

    return res;
}


QImage ImageManip::Manip::TriangleCirconscrit(int imageWidth, int imageHeight, int seed)
{
    RGen gen;
    gen.seed(seed);

    std::uniform_real_distribution<double> pointDist(0, 1);

    Types::Point pa = {pointDist(gen) * imageWidth, pointDist(gen) * imageHeight};
    Types::Point pb = {pointDist(gen) * imageWidth, pointDist(gen) * imageHeight};
    Types::Point pc = {pointDist(gen) * imageWidth, pointDist(gen) * imageHeight};
//
//    double D = 2 * (pa.x * (pb.y -  pc.y) + pb.x * (pc.y - pa.y) + pc.x * (pa.y - pb.y));
//
//    double centerX = 1 / D * (
//            (std::pow(pa.x, 2) + std::pow(pa.y, 2)) * (pb.y - pc.y) +
//            (std::pow(pb.x, 2) + std::pow(pb.y, 2)) * (pc.y - pa.y) +
//            (std::pow(pc.x, 2) + std::pow(pc.y, 2)) * (pa.y - pb.y)
//            );
//
//    double centerY = 1 / D * (
//            (std::pow(pa.x, 2) + std::pow(pa.y, 2)) * (pc.x - pb.x) +
//            (std::pow(pb.x, 2) + std::pow(pb.y, 2)) * (pa.x - pc.x) +
//            (std::pow(pc.x, 2) + std::pow(pc.y, 2)) * (pb.x - pa.x)
//            );
//
//    Point circonscritCenter = {centerX, centerY};
//
//    int radius = Distance(pa.x, pa.y, circonscritCenter.x,
//                          circonscritCenter.y);
    auto circum = CircumCircle(pa, pb, pc);

    QImage image(imageWidth, imageHeight, QImage::Format_RGBA64);

    QPainter painter(&image);

    painter.drawPoint(pa.x, pa.y);
    painter.drawPoint(pb.x, pb.y);
    painter.drawPoint(pc.x, pc.y);

    painter.drawLine(pa.x, pa.y, pb.x, pb.y);
    painter.drawLine(pb.x, pb.y, pc.x, pc.y);
    painter.drawLine(pc.x, pc.y, pa.x, pa.y);

    painter.drawEllipse(QPoint(circum.x, circum.y),
                        static_cast<int>(circum.radius),
                        static_cast<int>(circum.radius));

    painter.end();

    return image;
}


QImage  ImageManip::Manip::DelaunayNoise(
        int imageWidth, int imageHeight,
        int frequency, int seed,
        bool colored, bool tileable
)
{
    QImage res(imageWidth, imageHeight, QImage::Format_RGBA64);

    // ----------------------------------------------------------------------
    // Generate the points first.

    // Generate a grid with points.
    Grid grid(frequency, seed);
    if (tileable)
    {
        grid.Tile();
    }

    QPainter painter(&res);
    painter.setRenderHint(QPainter::Antialiasing);

    Types::Triangle superTriangle = {
            Types::Point(-imageWidth * 4, -imageHeight),
            Types::Point(imageWidth * 5, -imageHeight),
            Types::Point(imageWidth / 2, 8 * imageHeight)
    };

    double x = 0;
    double y = 0;
    double cellWidth = imageWidth / frequency;
    double cellHeight = imageHeight / frequency;
    if (tileable)
    {
        x = -cellWidth;
        y = -cellHeight;
    }

    auto points = grid.MapPoints(x, y, cellWidth, cellHeight);

    auto triangles = DelaunayTriangulation(points, superTriangle);
    for (const Types::Triangle& triangle : triangles)
    {
        QPoint pnts[] = {QPoint(triangle.a.x, triangle.a.y),
                         QPoint(triangle.b.x, triangle.b.y),
                         QPoint(triangle.c.x, triangle.c.y)};
        painter.drawPolygon(pnts, 3, Qt::OddEvenFill);
    }

    RGen gen;
    gen.seed(seed);
    std::uniform_int_distribution<int> colorDist(0, 255);

    std::map<double, QColor> triangleColors;

    painter.setPen(Qt::NoPen);
    for (const auto& triangle : triangles)
    {
        double radius = CircumCircle(triangle.a, triangle.b, triangle.c).radius;

        double hashRadius = ROUND(radius, 5);

        if (triangleColors.find(hashRadius) == triangleColors.end())
        {
            QColor color;
            if (colored)
            {

                color = {
                        colorDist(gen),
                        colorDist(gen),
                        colorDist(gen)
                };
            }
            else
            {
                int grey = colorDist(gen);
                color = {grey, grey, grey};
            }

            triangleColors[hashRadius] = color;
        }

        QColor triangleColor = triangleColors.at(hashRadius);
        painter.setBrush(triangleColor);

        QPoint pnts[] = {QPoint(triangle.a.x, triangle.a.y),
                         QPoint(triangle.b.x, triangle.b.y),
                         QPoint(triangle.c.x, triangle.c.y)};
        painter.drawPolygon(pnts, 3, Qt::OddEvenFill);
    }

    return res;
}






QImage ImageManip::Manip::VoronoiNoise(
        int imageWidth, int imageHeight,
        int frequency, int seed, int lineWidth,
        bool filled, bool colored, bool tileable)
{
    QImage res(imageWidth, imageHeight, QImage::Format_RGBA64);
    res.fill(Qt::white);
    // ----------------------------------------------------------------------
    // Generate the points first.

    // Generate a grid with points.
    Grid grid(frequency, seed);
    if (tileable)
    {
        grid.TileRepeat();
    }

    QPainter painter(&res);
    painter.setRenderHint(QPainter::Antialiasing);

    Types::Triangle superTriangle = {
            Types::Point(-imageWidth * 4, -imageHeight),
            Types::Point(imageWidth * 5, -imageHeight),
            Types::Point(imageWidth / 2, 8 * imageHeight)
    };

    double x = 0;
    double y = 0;
    double cellWidth = imageWidth / frequency;
    double cellHeight = imageHeight / frequency;
    if (tileable)
    {
        x = -imageWidth / 2;
        y = -imageHeight / 2;
    }

    auto points = grid.MapPoints(x, y, cellWidth, cellHeight);

    auto triangles = DelaunayTriangulation(points, superTriangle);

    RGen gen;
    gen.seed(seed);
    std::uniform_int_distribution<int> colorDist(0, 255);

    // order points using an angle between (1, 0) vector to
    // specified vector. It should order around the point...
    // I guess...
    std::unordered_map<Types::Point, std::vector<Types::Triangle>> trianglePoints;
    for (const Types::Triangle& triangle : triangles)
    {
        for (const Types::Point& point : triangle.Points())
        {
            auto roundPoint = point.Round(5);
            if (trianglePoints.find(roundPoint) == trianglePoints.end())
                trianglePoints[roundPoint] = std::vector<Types::Triangle>();
            trianglePoints.at(roundPoint).push_back(triangle);
        }
    }

    std::vector<Types::Polygon> polygons;
    std::set<size_t> hashes;
    for (const auto& pointTriangle: trianglePoints)
    {
        Types::Polygon p;

        for (const Types::Triangle& t : pointTriangle.second)
        {
            auto circum = CircumCircle(t.a, t.b, t.c);
            p.points.emplace_back(circum.x, circum.y);
        }

        p.ComputeCenter();
        p.Sort();

        polygons.push_back(p);
        hashes.insert(p.Hash());
    }

    painter.setPen(Qt::NoPen);

    std::unordered_map<size_t, QColor> polygonColors;

    for (const Types::Polygon& polygon : polygons)
    {
        if (polygon.points.size() < 3)
            continue;

        if (filled)
        {
            size_t hash = polygon.Hash();
            if (polygonColors.find(hash) == polygonColors.end())
            {
                if (colored)
                {
                    polygonColors[hash] = QColor(
                            colorDist(gen),
                            colorDist(gen),
                            colorDist(gen)
                    );
                }
                else
                {
                    int c = colorDist(gen);
                    polygonColors[hash] = QColor(c, c, c);
                }
            }

            QPen ppen;
            ppen.setWidth(1);
            ppen.setColor(polygonColors.at(hash));
            painter.setPen(ppen);
            painter.setBrush(polygonColors.at(hash));
        }

        else
        {
            QPen ppen;
            ppen.setWidth(lineWidth);
            painter.setPen(ppen);
            painter.setBrush(Qt::NoBrush);
        }

        painter.drawPolygon(polygon.ToQPolygon(), Qt::OddEvenFill);
    }

    return res;
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


//double OrderAngle(ImageManip::Manip::Point& p1,
//                  ImageManip::Manip::Point& p2)
//{
//
//}


QImage ImageManip::Manip::CircallyOrdered(
        int imageWidth, int imageHeight,
        int frequency, int seed)
{
    RGen gen;
    gen.seed(seed);

    std::uniform_real_distribution<double> dist(0, 1);

    std::vector<Types::Point> points;

    double sumX = 0;
    double sumY = 0;
    for (unsigned int i = 0; i < frequency; i++)
    {
        double x = std::round(dist(gen) * imageWidth);
        double y = std::round(dist(gen) * imageHeight);
        points.emplace_back(x, y);

        sumX += x;
        sumY += y;
    }

    Types::Point center(sumX / frequency, sumY / frequency);

    QImage res(imageWidth + 550, imageHeight + 550, QImage::Format_RGBA64);
    res.fill(Qt::black);

    QPainter painter(&res);

    struct Compare
    {
        Types::Point p;

        Compare(Types::Point center)
        {
            p = center;
        }

        bool operator()(const Types::Point& lhs, const Types::Point& rhs) const
        {
            Types::Point vec1 = lhs - p;
            Types::Point vec2 = rhs - p;

            Types::Point refVec = p + Types::Point(1, 0);

            double angle1 = Angle(refVec, vec1);
            if (lhs.y < p.y)
            {
                angle1 = 2 * Types::PI() - angle1;
            }

            double angle2 = Angle(refVec, vec2);
            if (rhs.y < p.y)
            {
                angle2 = 2 * Types::PI() - angle2;
            }

            return angle1 > angle2;
        }
    };

    std::sort(points.begin(), points.end(), Compare(center));

    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);

    painter.drawEllipse(QPoint(center.x, center.y), 5, 5);

    painter.setPen(Qt::red);
    painter.setBrush(Qt::NoBrush);

    painter.setFont(QFont("sans", 20));
    painter.drawText(center.x + 5, center.y + 5, ("Center (" + std::to_string((int)center.x) + ", " + std::to_string((int)center.y) + ")").c_str());

    painter.setBrush(Qt::green);

    unsigned int pi = 0;
    for (const auto& p : points)
    {
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPoint(p.x, p.y), 5, 5);

        pi++;
    }

    painter.setPen(Qt::NoPen);

    painter.end();

    return res;
}


double SmoothStep(double value)
{
    return 6 * std::pow(value, 5) - 15 * std::pow(value, 4) + 10 * std::pow(value, 3);
}


double Lerp(double v1, double v2, double t)
{
    return v1 + t * (v2 - v1);
}


auto GetGradient(const ImageManip::Manip::PerlinNoiseOptions& options, int x, int y)
{
    auto pe1 = options.permutations.at(y % 256);
    auto pe2 = ((x + pe1) % 256);

    return options.vectors.at(options.permutations.at(pe2));
};


double ImageManip::Manip::PerlinNoise(
        double x, double y,
        const PerlinNoiseOptions& options
)
{
    std::vector<std::vector<Types::Point>> meshVectors;



    double x0 = std::floor(x);
    double y0 = std::floor(y);
    double x1 = x0 + 1.0;
    double y1 = y0 + 1.0;

    Types::Point gridVectorTopLeft = GetGradient(options, (int)x0, (int)y0);
    Types::Point gridVectorTopRight = GetGradient(options, (int)x1, (int)y0);
    Types::Point gridVectorBottomLeft = GetGradient(options, (int)x0, (int)y1);
    Types::Point gridVectorBottomRight = GetGradient(options, (int)x1, (int)y1);

    Types::Point gridTopLeftToPoint = Types::Normalized({
            x - x0, y - y0
    });

    Types::Point gridTopRightToPoint = Types::Normalized({
            x - x1, y - y0
    });

    Types::Point gridBottomLeftToPoint = Types::Normalized({
            x - x0, y - y1
    });

    Types::Point gridBottomRightToPoint = Types::Normalized({
            x - x1, y - y1
    });

    double scalarTopLeft = ScalarProduct(
            gridVectorTopLeft,
            gridTopLeftToPoint
    );

    double scalarTopRight = ScalarProduct(
            gridVectorTopRight,
            gridTopRightToPoint
    );

    double scalarBottomLeft = ScalarProduct(
            gridVectorBottomLeft,
            gridBottomLeftToPoint
    );

    double scalarBottomRight = ScalarProduct(
            gridVectorBottomRight,
            gridBottomRightToPoint
    );

    double horizontalLerpFactor = SmoothStep((x - x0));
    double verticalLerpFactor = SmoothStep((y - y0));

    double horizontalLerp = Lerp(scalarTopLeft, scalarTopRight, horizontalLerpFactor);

    double verticalLerp = Lerp(scalarBottomLeft, scalarBottomRight, horizontalLerpFactor);

    return Lerp(horizontalLerp, verticalLerp, verticalLerpFactor);
}


QImage ImageManip::Manip::FractalPerlinNoise(
        int imageWidth,
        int imageHeight,
        int octave,
        int frequency,
        double lacunarity,
        double persistence,
        int seed,
        ModifierFunction modifier
)
{
    RGen gen(seed);

    QImage perlinImage(imageWidth, imageHeight, QImage::Format_RGBA64);

    PerlinNoiseOptions options;

    DoubleDist dist(-1, 1);

    for (int v = 0; v < 256; v++)
    {
        options.vectors[v] = Types::Normalized({dist(gen), dist(gen)});
    }

    // Generate permutation table.
    NormalDist permutationDist(0, 256);
    RGen permutationGen(seed);

    for (int p=0; p < 256; p++)
    {
        options.permutations[p] = permutationDist(permutationGen);
    }

    for (int v = 256; v < 512; v++)
    {
        options.permutations[v] = options.permutations.at(v % 256);
    }

    for (int y = 0; y < imageHeight; y++)
    {
        for (int x = 0; x < imageWidth; x++)
        {

            double amplitude = 1.0;
            double amplitudeSum = 0.0;
            double octaveFrequency = frequency;

            double perlin = 0;
            for (int o = 0; o < octave; o++)
            {
                double ox = (double)x / ((double)imageWidth) * octaveFrequency;
                double oy = (double)y / ((double)imageHeight) * octaveFrequency;

                double octavePerlin = PerlinNoise(
                        ox, oy, options
                );

                perlin += octavePerlin * amplitude;
                amplitudeSum += amplitude;

                amplitude *= persistence;
                octaveFrequency *= lacunarity;
            }

            perlin = perlin / amplitudeSum;

            if (modifier)
            {
                perlin = modifier(perlin);
            }

            perlin = std::clamp(perlin, -1.0, 1.0);

            double c = 125.0;

            QColor color(c + perlin * c,
                         c + perlin * c,
                         c + perlin * c,
                         255);

            perlinImage.setPixelColor(
                    x, y, color

            );
        }
    }

    return perlinImage;
}


QImage ImageManip::Manip::FractalCloudNoise(
        int imageWidth,
        int imageHeight,
        int octave,
        int frequency,
        int octaveFrequencyFactor,
        double smoothLevel,
        double octavePersistence,
        int seed
)
{
//    auto modifier = [](double noise)
//    {
//        return 1.0 - std::pow(1.0 - noise, 2);
//    };

    auto modifier = [smoothLevel](double noise)
    {
        return std::abs(std::pow(noise, smoothLevel));
    };

    return FractalPerlinNoise(
        imageWidth,
        imageHeight,
        octave, frequency,
        octaveFrequencyFactor,
        octavePersistence,
        seed, modifier
    );
}
