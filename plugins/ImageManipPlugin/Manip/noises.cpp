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



ImageManip::Manip::Point::Point(double x_, double y_)
{
    x = x_;
    y = y_;
}


bool ImageManip::Manip::Point::operator==(const Point& other) const
{
    return (x == other.x && y == other.y);
}


ImageManip::Manip::Point ImageManip::Manip::Point::operator-(const Point& other) const
{
    return {x - other.x, y - other.y};
}


ImageManip::Manip::Point ImageManip::Manip::Point::operator+(const Point& other) const
{
    return {x + other.x, y + other.y};
}


#define ROUND(number, decimals) std::round(number * std::pow(10, decimals)) / std::pow(10, decimals)


ImageManip::Manip::Point ImageManip::Manip::Point::Round(unsigned int decimals) const
{
    return {ROUND(x, decimals), ROUND(y, decimals)};
}


ImageManip::Manip::Segment::Segment(Point p1_, Point p2_)
{
    p1 = p1_;
    p2 = p2_;
}


bool ImageManip::Manip::Segment::operator==(const Segment& other) const
{
    return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
}


ImageManip::Manip::Triangle::Triangle(const Point& a_, const Point& b_, const Point& c_)
{
    a = a_;
    b = b_;
    c = c_;
}


bool ImageManip::Manip::Triangle::operator==(const Triangle& other) const
{
    auto segments = Segments();
    auto segmentsOther = Segments();

    for (const auto& seg : Segments())
    {
        auto iter = std::find(segmentsOther.begin(), segmentsOther.end(), seg);
        if (iter == segmentsOther.end())
        {
            return false;
        }

        segmentsOther.erase(iter);
    }

    return true;
}


std::vector<ImageManip::Manip::Segment> ImageManip::Manip::Triangle::Segments() const
{
    return {
            Segment(a, b),
            Segment(b, c),
            Segment(c, a)
    };
}


bool ImageManip::Manip::Triangle::HasSegment(const Segment& other) const
{
    auto segments = Segments();
    for (const auto& segment : segments)
    {
        if (segment == other)
        {
            return true;
        }
    }

    return false;
}


std::vector<ImageManip::Manip::Point> ImageManip::Manip::Triangle::Points() const
{
    return {a, b, c};
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


std::vector<ImageManip::Manip::Point> ImageManip::Manip::Grid::MapPoints(
        double x, double y, double cellWidth,
        double cellHeight) const
{
    std::vector<Point> points;

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


double ImageManip::Manip::Distance(double x1, double y1, double x2, double y2)
{
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}


ImageManip::Manip::Point Normalized(const ImageManip::Manip::Point vector)
{
    double vNorm = std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2));

    return {vector.x / vNorm, vector.y / vNorm};
}


double ImageManip::Manip::NormalizedAngle(
        const ImageManip::Manip::Point& vector1,
        const ImageManip::Manip::Point& vector2
)
{
//    double v1Length = Distance(p1.x, p1.y, p2.x, p2.y);
//    double v2Length = Distance(p1.x, p1.y, p3.x, p3.y);

    double v1Norm = std::sqrt(std::pow(vector1.x, 2) + std::pow(vector1.y, 2));
    double v2Norm = std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2));

    double scalarProduct = vector1.x * vector2.x + vector1.y * vector2.y;

    double p = scalarProduct / (v1Norm * v2Norm);

    double ac = std::acos(p);
//    return std::acos((v1.x * v2.x) + (v1.y * v2.y));
    return ac;
}


double ImageManip::Manip::Angle(
        const ImageManip::Manip::Point& vector1,
        const ImageManip::Manip::Point& vector2
)
{
//    double v1Length = Distance(p1.x, p1.y, p2.x, p2.y);
//    double v2Length = Distance(p1.x, p1.y, p3.x, p3.y);

    double v1Norm = std::sqrt(std::pow(vector1.x, 2) + std::pow(vector1.y, 2));
    double v2Norm = std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2));

    double scalarProduct = vector1.x * vector2.x + vector1.y * vector2.y;

    double p = scalarProduct / (v1Norm * v2Norm);

    double ac = std::acos(p);
//    return std::acos((v1.x * v2.x) + (v1.y * v2.y));
    return ac;
}


#define PI 3.14159265358979323846


double ImageManip::Manip::TrigoAngle(
        const Point& vector
)
{
    double angle = Angle({1, 0}, vector);

    if (vector.y < 0)
    {
        angle = (2 * PI) - angle;
    }

    return angle;
}


double ImageManip::Manip::TrigoAngle(
        const Point& root, const Point& vector
)
{
    double angle = Angle(root + Point(1, 0), vector);

    if (vector.y < root.y)
    {
        angle = (2 * PI) - angle;
    }

    double deg = angle * 57.2958;
    return angle;
}


double PointRootAngle(const ImageManip::Manip::Point& root,
                      const ImageManip::Manip::Point& point)
{
    ImageManip::Manip::Point vector = point - root;
    ImageManip::Manip::Point refVec = (root + ImageManip::Manip::Point(1, 0)) - root;

    double angle = Angle(refVec, vector);
    if (point.y < root.y)
    {
        angle = 2 * PI - angle;
    }

    return angle;
}


bool _Matches(ImageManip::Manip::Triangle t1,
              ImageManip::Manip::Triangle t2)
{
    for (const ImageManip::Manip::Segment& seg : t1.Segments())
    {
        if (t2.HasSegment(seg))
            return true;
    }

    return false;
}


bool _PointMatch(ImageManip::Manip::Triangle t1,
                 ImageManip::Manip::Triangle t2)
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


bool ImageManip::Manip::IsInsideCircle(const Circle& circle, const ImageManip::Manip::Point& p)
{
    return Distance(p.x, p.y, circle.x, circle.y) <= circle.radius;
}


ImageManip::Manip::Circle ImageManip::Manip::CircumCircle(
        const ImageManip::Manip::Point& pa,
        const ImageManip::Manip::Point& pb,
        const ImageManip::Manip::Point& pc)
{
    double D = 2 * (pa.x * (pb.y -  pc.y) + pb.x * (pc.y - pa.y) + pc.x * (pa.y - pb.y));

    double centerX = 1 / D * (
            (std::pow(pa.x, 2) + std::pow(pa.y, 2)) * (pb.y - pc.y) +
            (std::pow(pb.x, 2) + std::pow(pb.y, 2)) * (pc.y - pa.y) +
            (std::pow(pc.x, 2) + std::pow(pc.y, 2)) * (pa.y - pb.y)
    );

    double centerY = 1 / D * (
            (std::pow(pa.x, 2) + std::pow(pa.y, 2)) * (pc.x - pb.x) +
            (std::pow(pb.x, 2) + std::pow(pb.y, 2)) * (pa.x - pc.x) +
            (std::pow(pc.x, 2) + std::pow(pc.y, 2)) * (pb.x - pa.x)
    );

    double radius = Distance(pa.x, pa.y, centerX,centerY);

    return {centerX, centerY, radius};
}


std::vector<ImageManip::Manip::Triangle> AddPoint(ImageManip::Manip::Point point, std::vector<ImageManip::Manip::Triangle> triangles)
{
    std::vector<ImageManip::Manip::Triangle> res;
    std::vector<ImageManip::Manip::Segment> edges;

    for (const ImageManip::Manip::Triangle& triangle : triangles)
    {
        auto circum = CircumCircle(triangle.a, triangle.b, triangle.c);
        if  (IsInsideCircle(circum, point))
        {
            for (const ImageManip::Manip::Segment& edge : triangle.Segments())
            {
                edges.push_back(edge);
            }
        }
        else
        {
            res.push_back(triangle);
        }
    }

    std::vector<ImageManip::Manip::Segment> polygon;
    for (const auto& edge : edges)
    {
        if (std::count(edges.begin(), edges.end(), edge) > 1)
            continue;
        res.emplace_back(edge.p1, edge.p2, point);
    }

    return res;
}


std::vector<ImageManip::Manip::Triangle> ImageManip::Manip::DelaunayTriangulation(
        const std::vector<Point>& points, const Triangle& superTriangle
)
{
    std::vector<Triangle> triangles = {superTriangle};

    for (const auto& point : points)
    {
        triangles = AddPoint(point, triangles);
    }

    std::vector<Triangle> res;
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

    Point pa = {pointDist(gen) * imageWidth, pointDist(gen) * imageHeight};
    Point pb = {pointDist(gen) * imageWidth, pointDist(gen) * imageHeight};
    Point pc = {pointDist(gen) * imageWidth, pointDist(gen) * imageHeight};
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

    Triangle superTriangle = {
            Point(-imageWidth * 4, -imageHeight),
            Point(imageWidth * 5, -imageHeight),
            Point(imageWidth / 2, 8 * imageHeight)
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
    for (const Triangle& triangle : triangles)
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


ImageManip::Manip::Point ImageManip::Manip::AveragePoint(
        std::vector<ImageManip::Manip::Point> points)
{
    double sumX = 0;
    double sumY = 0;
    for (const Point& p : points)
    {
        sumX += p.x;
        sumY += p.y;
    }

    return {sumX / static_cast<double>(points.size()),
            sumY / static_cast<double>(points.size())};
}


size_t HashPolygon(std::vector<ImageManip::Manip::Point> polygon)
{
    double sumX = 0;
    double sumY = 0;
    for (const auto& point : polygon)
    {
        sumX += point.x;
        sumY += point.y;
    }

    ImageManip::Manip::Point center(
            sumX / (double)polygon.size(),
            sumY / (double)polygon.size()
            );

    size_t hash = 0;
    for (const auto& point : polygon)
    {
        ImageManip::Manip::Point vec = Normalized(point  - center);
        hash_combine(hash, vec);
    }

    return hash;
}


void ImageManip::Manip::Polygon::Sort()
{
    auto cmp = [this](const Point& lhs, const Point& rhs)
    {
        return PointRootAngle(center, lhs) < PointRootAngle(center, rhs);
    };

    std::sort(points.begin(), points.end(), cmp);
}


void ImageManip::Manip::Polygon::ComputeCenter()
{
    center = AveragePoint(points);
}


size_t ImageManip::Manip::Polygon::Hash() const
{
    Polygon normalized = Normalized();

    size_t hash = 0;
    for (const Point& p : normalized.points)
    {
        hash_combine(hash, p.Round(3));
    }

    return hash;
}


ImageManip::Manip::Polygon ImageManip::Manip::Polygon::Normalized() const
{
    Polygon p;
    p.center = {0, 0};

    for (const auto& point : points)
    {
        p.points.push_back(point - center);
    }

    return p;
}


bool ImageManip::Manip::Polygon::operator==(const Polygon& other) const
{
    return (Hash() == other.Hash());
}


QPolygon ImageManip::Manip::Polygon::ToQPolygon() const
{
    QPolygon p;

    for (const auto& pnt : points)
    {
        p.emplace_back(pnt.x, pnt.y);
    }

    return p;
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

    Triangle superTriangle = {
            Point(-imageWidth * 4, -imageHeight),
            Point(imageWidth * 5, -imageHeight),
            Point(imageWidth / 2, 8 * imageHeight)
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
    std::unordered_map<Point, std::vector<Triangle>> trianglePoints;
    for (const Triangle& triangle : triangles)
    {
        for (const Point& point : triangle.Points())
        {
            auto roundPoint = point.Round(5);
            if (trianglePoints.find(roundPoint) == trianglePoints.end())
                trianglePoints[roundPoint] = std::vector<Triangle>();
            trianglePoints.at(roundPoint).push_back(triangle);
        }
    }

    std::vector<Polygon> polygons;
    std::set<size_t> hashes;
    for (const auto& pointTriangle: trianglePoints)
    {
        Polygon p;

        for (const Triangle& t : pointTriangle.second)
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

    for (const Polygon& polygon : polygons)
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

    std::vector<Point> points;

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

    Point center(sumX / frequency, sumY / frequency);

    QImage res(imageWidth + 550, imageHeight + 550, QImage::Format_RGBA64);
    res.fill(Qt::black);

    QPainter painter(&res);

    struct Compare
    {
        Point p;

        Compare(Point center)
        {
            p = center;
        }

        bool operator()(const Point& lhs, const Point& rhs) const
        {
            Point vec1 = lhs - p;
            Point vec2 = rhs - p;

            Point refVec = p + Point(1, 0);

            double angle1 = Angle(refVec, vec1);
            if (lhs.y < p.y)
            {
                angle1 = 2 * PI - angle1;
            }

            double angle2 = Angle(refVec, vec2);
            if (rhs.y < p.y)
            {
                angle2 = 2 * PI - angle2;
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

        painter.setPen(Qt::green);
        std::string text = std::to_string(pi);

        text += " (" + std::to_string(p.x) + ", " + std::to_string(p.y) + "): " + std::to_string(PointRootAngle(center, p));
        painter.setFont(QFont("sans", 20));
        painter.drawText(p.x + 5, p.y + 5, text.c_str());
        pi++;
    }

    painter.setPen(Qt::NoPen);

    painter.end();

    return res;
}
