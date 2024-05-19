#include "BezierFunc.h"
#include <cmath>
#include <algorithm>


void Gex::InputRel::Point::operator=(Point other)
{
    Swap(other);
}


void Gex::InputRel::Point::Swap(const Point& other)
{
    x = other.x;
    y = other.y;
}


Gex::InputRel::Point Gex::InputRel::operator*(double f,
        const Gex::InputRel::Point& p)
{
    return {f * p.x, f* p.y};
}


Gex::InputRel::Point Gex::InputRel::operator+(
        const Gex::InputRel::Point& p1,
        const Gex::InputRel::Point& p2)
{
    return {p1.x + p2.x,  p1.y + p2.y};
}


bool Gex::InputRel::operator==(const Gex::InputRel::Point& p1,
        const Gex::InputRel::Point& p2)
{
    return (p1.x == p2.x && p1.y == p2.y);
}



Gex::InputRel::ControlPoints Gex::InputRel::BezierCurve::ControlPoints(
        const Point& point, bool& found) const
{
    auto where = points.find(point);

    found = (where != points.end());
    if (!found)
        return {};

    return where->second;
}


Gex::InputRel::Points Gex::InputRel::BezierCurve::CurvePoints() const
{
    Points res;
    for (auto p : points)
    {
        res.push_back(p.first);
    }

    return res;
}


bool Gex::InputRel::BezierCurve::MovePoint(Point source, Point dest)
{
    for (auto p: points)
    {
        if (p.first == source)
        {
            auto pair = *p;
            points.emplace()
        }
    }
}



Gex::InputRel::PointsMap Gex::InputRel::BezierCurve::CurvePointsMap() const
{
    return points;
}


//Gex::InputRel::ControlPoints Gex::InputRel::BezierCurve::ControlPoints(
//        const Point& point) const
//{
//    return points.at(point);
//}


void Gex::InputRel::BezierCurve::AddPoint(Point point)
{
    bool previous = false;
    bool next = false;
    Points adjacents = AdjacentPoints(point.x, previous, next);

    ControlPoints cpoints = {{point.x - 1, point.y},
                             {point.x + 1, point.y}};

    if (next)
    {
        // Insert point at first place.
        cpoints.cp2 = {(adjacents.at(0).x - point.x) / 2,
                       (adjacents.at(0).y - point.y) / 2};
    }
    else if (previous)
    {
        // Insert point at first place.
        cpoints.cp1 = {(point.x - adjacents.at(0).x) / 2,
                       (point.y - adjacents.at(0).y) / 2};
    }

    points[point] = cpoints;
}


void Gex::InputRel::BezierCurve::RemovePoint(Point point)
{
    auto where = points.find(point);
    if (where  == points.end())
        return;

    points.erase(point);
}


Gex::InputRel::Points Gex::InputRel::BezierCurve::AdjacentPoints(
        const double& x, bool& foundPrev, bool& foundPost) const
{

    Point prev;
    double pvd = 1;

    Point post;
    double psd = -1;

    for (auto p : points)
    {
        double distx = x - p.first.x;
        if (distx < 0 && pvd < 0 && distx > pvd)
        {
            pvd = distx;
            prev = p.first;
        }
        else if (distx > 0 && psd > 1 && distx < psd)
        {
            psd = distx;
            post = p.first;
        }
        else if (distx == 0)
        {
            pvd = distx;
            prev = p.first;
        }
    }

    foundPrev = false;
    foundPost = false;

    Points adjacent;
    if (pvd <= 0)
    {
        adjacent.push_back(prev);
        foundPrev = true;
    }
    if (psd >= 0)
    {
        adjacent.push_back(post);
        foundPost = true;
    }

    return adjacent;
}


double Gex::InputRel::BezierCurve::y(const double& x) const
{
    bool previous = false;
    bool next = false;
    Points adjacent = AdjacentPoints(x, previous, next);

    if (adjacent.size() == 1)
    {
        return adjacent.at(0).y;
    }
    //        |
    // x -- y -- x -- y -- x -- y -- x

    Point previousPoint = adjacent.at(0);
    Point nextPoint = adjacent.at(1);

    ControlPoints prevCPoints1 = points.at(previousPoint);
    ControlPoints nextCPoints2 = points.at(nextPoint);

    double u = x / (nextPoint.x - previousPoint.x);

    return (std::pow(1-u, 3) * previousPoint.y +
            u * prevCPoints1.cp2.y * (3 * std::pow(1-u, 2)) +
            nextCPoints2.cp1.y * (3 * (1-u) * std::pow(u, 2)) +
            nextPoint.y * std::pow(u, 3));
}
