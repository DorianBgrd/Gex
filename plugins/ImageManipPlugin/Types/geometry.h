#ifndef GEX_TYPE_GEOMETRY_H
#define GEX_TYPE_GEOMETRY_H

#include <map>
#include <vector>
#include <QPoint>
#include <QPointF>
#include <QPolygon>

#include "Tsys/include/tsys.h"


#include "../export.h"


namespace ImageManip::Types
{
#define ROUND(number, decimals) std::round(number * std::pow(10, decimals)) / std::pow(10, decimals)


    double PI();

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

        QPoint ToQPoint() const;

        QPointF ToQPointF() const;
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


    bool IsInsideCircle(const Circle& circle, const Point& p);


    Circle CircumCircle(const Point& pa,
                        const Point& pb,
                        const Point& pc);


    double Distance(double x1, double y1, double x2, double y2);


    double NormalizedAngle(const Point& vector1,
                           const Point& vector2);

    double Angle(const Point& vector1,
                 const Point& vector2);


    double TrigoAngle(const Point& vector);

    double TrigoAngle(const Point& root,
                      const Point& vector);


    Point AveragePoint(std::vector<Point> points);


    size_t HashPolygon(std::vector<Point> polygon);


    struct Polygon
    {
        Types::Point center;
        std::vector<Types::Point> points;
        std::vector<Types::Triangle> triangles;

        void Sort();

        void ComputeCenter();

        size_t Hash() const;

        Polygon Normalized() const;

        bool operator==(const Polygon& other) const;

        QPolygon ToQPolygon() const;
    };



    struct Plugin_API PointHandler: public TSys::TypeHandler
    {
        void SerializeValue(std::any v, rapidjson::Value& value,
                            rapidjson::Document& document)
                            const override;

        std::any DeserializeValue(std::any v, rapidjson::Value& value)
                                  const override;

        void SerializeConstruction(std::any v, rapidjson::Value& value,
                                   rapidjson::Document& document)
                                   const override;

        std::any DeserializeConstruction(rapidjson::Value& value) const override;

        std::any InitValue() const override;

        bool CompareValue(std::any, std::any) const override;

        std::any FromPython(boost::python::object) const override;

        boost::python::object ToPython(std::any) const override;

        std::any CopyValue(std::any source) const override;

        size_t Hash() const override;

        std::string Name() const override;

        std::string PythonName() const override;

        std::string ApiName() const override;

        size_t ValueHash(std::any val) const override;
    };
}


template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


template<>
struct std::hash<ImageManip::Types::Point>
{
    std::size_t operator()(const ImageManip::Types::Point& p) const noexcept
    {
        std::size_t h1 = std::hash<double>{}(p.x);
        std::size_t h2 = std::hash<double>{}(p.y);
        return h1 ^ (h2 << 1);
    }
};


template<>
struct std::hash<ImageManip::Types::Triangle>
{
    std::size_t operator()(const ImageManip::Types::Triangle& p) const noexcept
    {
        auto circum = ImageManip::Types::CircumCircle(p.a, p.b, p.c);

        size_t hash = std::hash<double>()(circum.radius);
        return hash;
    }
};



template<>
struct std::hash<ImageManip::Types::Segment>
{
    std::size_t operator()(const ImageManip::Types::Segment& p) const noexcept
    {
        std::size_t h1 = std::hash<ImageManip::Types::Point>{}(p.p1);
        std::size_t h2 = std::hash<ImageManip::Types::Point>{}(p.p2);

        hash_combine(h1, h2);
        return h1;
    }
};


#endif //GEX_TYPE_GEOMETRY_H