#ifndef GEX_BEZIERFUNC_H
#define GEX_BEZIERFUNC_H

#include <vector>
#include <map>

namespace Gex::InputRel
{
    struct Point
    {
        double x=0;
        double y=0;

        void operator=(Point other);

        void Swap(const Point& other);
    };


    struct ControlPoints
    {
        Point cp1;
        Point cp2;
    };


    static Point operator*(double f, const Point& p);

    static Point operator+(const Point& p1, const Point& p2);

    static bool operator==(const Point& p1, const Point& p2);


    struct PointCompare
    {
        bool operator()(const Gex::InputRel::Point& point1,
                        const Gex::InputRel::Point& point2)
                        const
        {
            return (point1.x < point2.x);
        }
    };

    typedef std::vector<Point> Points;
    typedef std::array<Point, 2> PointTuple;
    typedef std::map<Point, ControlPoints, PointCompare> PointsMap;


    enum class Side
    {
        Left,
        Right
    };

    class BezierCurve
    // Bezier curve acts as bezier curve segments of 3 points.
    // This means that adding a point automatically adds a
    // control point between the previous point and the
    // added one.
    {
        // Curve point list.
        PointsMap points;

    public:
        // Returns all curve points.
//        Points CurvePoints() const;

        // Returns all control points.
        ControlPoints ControlPoints(const Point& point, bool& found) const;

        Points CurvePoints() const;

        bool MovePoint(Point source, Point dest);

        bool MovePoint(double sx, double sy, double dx, double dy);

        bool MoveControlPoint(Point point, Point source, Point dest,
                              Side side=Side::Left);

        PointsMap CurvePointsMap() const;

        // Adds a new point to the curve.
        void AddPoint(Point point);

        // Removes point from the curve.
        void RemovePoint(Point point);

    public:
        Points AdjacentPoints(const double& x, bool& foundPrev,
                              bool& foundPost) const;

        double y(const double& x) const;
    };

}

#endif //GEX_BEZIERFUNC_H
