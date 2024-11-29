#ifndef GEX_NOISES_H
#define GEX_NOISES_H

#include <QPixmap>
#include <vector>


namespace ImageManip::Manip
{

    QImage RandomNoise(int width, int height, int seed,
                       bool blackAndWhite=true,
                       bool alpha=false);

    struct Point
    {
        double x = 0;
        double y = 0;

        Point() = default;

        Point(const Point& other): Point(other.x, other.y) {}

        Point(double x_, double y_);

        bool operator==(const Point& other) const;

//        Point& operator=(const Point& other) = default;

        Point operator-(const Point& other) const;

        Point operator+(const Point& other) const;

        Point Round(unsigned int decimals) const;
    };


    struct Segment
    {
        Point p1;
        Point p2;

        Segment() = default;

        Segment(Point p1_, Point p2_);

        bool operator==(const Segment& other) const;
    };


    struct Triangle
    {
        Point a;
        Point b;
        Point c;

        Triangle() = default;

        Triangle(const Point& a, const Point& b, const Point& c);

        bool operator==(const Triangle& other) const;

        std::vector<Segment> Segments() const;

        bool HasSegment(const Segment& other) const;

        std::vector<Point> Points() const;
    };


    struct Circle
    {
        double x;
        double y;
        double radius;
    };


    bool IsInsideCircle(const Circle& circle, const ImageManip::Manip::Point& p);


    Circle CircumCircle(const ImageManip::Manip::Point& pa,
                        const ImageManip::Manip::Point& pb,
                        const ImageManip::Manip::Point& pc);


    double Distance(double x1, double y1, double x2, double y2);


    double NormalizedAngle(const ImageManip::Manip::Point& vector1,
                           const ImageManip::Manip::Point& vector2);

    double Angle(const ImageManip::Manip::Point& vector1,
                 const ImageManip::Manip::Point& vector2);


    double TrigoAngle(const ImageManip::Manip::Point& vector);

    double TrigoAngle(const ImageManip::Manip::Point& root,
                      const ImageManip::Manip::Point& vector);


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

        std::vector<Point> MapPoints(
                double x, double y, double cellWidth,
                double cellHeight) const;

        int ColumnCount() const;

        int RowCount() const;
    };


    std::vector<Triangle> DelaunayTriangulation(
            const std::vector<Point>& points,
            const Triangle& superTriangle
            );

    QImage DelaunayNoise(int imageWidth,
                         int imageHeight,
                         int frequency,
                         int seed,
                         bool colored=false,
                         bool tileable=true
                         );


    Point AveragePoint(std::vector<Point> points);

    struct Polygon
    {
        Point center;
        std::vector<Point> points;
        std::vector<Triangle> triangles;

        void Sort();

        void ComputeCenter();

        size_t Hash() const;

        Polygon Normalized() const;

        bool operator==(const Polygon& other) const;

        QPolygon ToQPolygon() const;
    };

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


template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


template<>
struct std::hash<ImageManip::Manip::Point>
{
    std::size_t operator()(const ImageManip::Manip::Point& p) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(p.x);
        std::size_t h2 = std::hash<int>{}(p.y);
        return h1 ^ (h2 << 1);
    }
};


template<>
struct std::hash<ImageManip::Manip::Triangle>
{
    std::size_t operator()(const ImageManip::Manip::Triangle& p) const noexcept
    {
        auto circum = ImageManip::Manip::CircumCircle(p.a, p.b, p.c);

        size_t hash = std::hash<double>()(circum.radius);
        return hash;
    }
};



template<>
struct std::hash<ImageManip::Manip::Segment>
{
    std::size_t operator()(const ImageManip::Manip::Segment& p) const noexcept
    {
        std::size_t h1 = std::hash<ImageManip::Manip::Point>{}(p.p1);
        std::size_t h2 = std::hash<ImageManip::Manip::Point>{}(p.p2);

        hash_combine(h1, h2);
        return h1;
    }
};


#endif //GEX_NOISES_H
