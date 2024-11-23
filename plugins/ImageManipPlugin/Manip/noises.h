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

        Point(double x_, double y_)
        {
            x = x_;
            y = y_;
        }

        bool operator==(const Point& other) const
        {
            return (x == other.x && y == other.y);
        }
    };


    struct Segment
    {
        Point p1;
        Point p2;

        Segment() = default;

        Segment(Point p1_, Point p2_)
        {
            p1 = p1_;
            p2 = p2_;
        }

        bool operator==(const Segment& other) const
        {
            return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
        }
    };


    struct Triangle
    {
        Point a;
        Point b;
        Point c;

        Triangle() = default;

        Triangle(Point a_, Point b_, Point c_)
        {
            a = a_;
            b = b_;
            c = c_;
        }

        bool operator==(const Triangle& other) const
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

        std::vector<Segment> Segments() const
        {
            return {
                    Segment(a, b),
                    Segment(b, c),
                    Segment(c, a)
            };
        }

        bool HasSegment(const Segment& other) const
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

        std::vector<Point> Points() const
        {
            return {a, b, c};
        }
    };


    std::vector<Triangle> DelaunayTriangulation(
            std::vector<Point> points,
            const Triangle& superTriangle
            );

    QImage DelaunayNoise(int imageWidth,
                         int imageHeight,
                         int frequency,
                         int seed,
                         bool tileable=true
                         );

    QImage VoronoiCells(int imageWidth,
                        int imageHeight,
                        int width,
                        int height,
                        int seed);

    QImage VoronoiNoise(int width, int height, int seed);

    QImage DirtNoise(int width, int height, int size,
                     int randomSize, int density,
                     int randomDensity, int seed);

    QImage TriangleCirconscrit(int imageWidth, int imageHeight, int seed);
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
        std::size_t h1 = std::hash<ImageManip::Manip::Point>{}(p.a);
        std::size_t h2 = std::hash<ImageManip::Manip::Point>{}(p.b);
        std::size_t h3 = std::hash<ImageManip::Manip::Point>{}(p.c);

        std::size_t rh1 = h1 ^ (h2 << 1);
        return rh1 ^ (h3 << 1);
    }
};



template<>
struct std::hash<ImageManip::Manip::Segment>
{
    std::size_t operator()(const ImageManip::Manip::Segment& p) const noexcept
    {
        std::size_t h1 = std::hash<ImageManip::Manip::Point>{}(p.p1);
        std::size_t h2 = std::hash<ImageManip::Manip::Point>{}(p.p2);
        return h1 ^ (h2 << 1);
    }
};


#endif //GEX_NOISES_H
